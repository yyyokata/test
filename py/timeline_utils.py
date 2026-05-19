import datetime
import json
import os
import tarfile
import tempfile


def _hash(event):
    """Returns hash(pid, tid, ts) of given `event`."""
    pid, tid = event.get("pid", -1), event.get("tid", -1)
    ts = int(event.get("ts", 0) * 1000)
    hash = f"{pid},{tid},{ts}"
    return hash


def _get_arg(event, arg_name, default=None):
    """Returns event["args"][arg_name]."""
    return event.get("args", {}).get(arg_name, default)


def _set_event_arg(event, arg_name, arg_value):
    """Sets event["args"][arg_name] to arg_value."""
    if "args" not in event:
        event["args"] = {}
    event["args"][arg_name] = arg_value


def _is_kernel(event):
    """Returns True if event is a gpu kernel event."""
    return event.get("cat", "") in ["kernel", "gpu_memcpy"]


class _CodeStack:
    """Stores a calling stack of 1 or more adjacent kernels."""

    def __init__(self, stack, pid, tid, ts, dur):
        self._pid = pid
        self._tid = tid
        self._ts = ts
        self._dur = dur
        self._layers = []
        self._is_erased = {}
        self._dur_after_merge = {}
        self._layer_kernels = []

        for i, code_line in enumerate(stack):
            self._is_erased[i] = False
            self._dur_after_merge[i] = dur
            self._layers.append(code_line)
            self._layer_kernels.append([])

    def set_leaf_kernel(self, kernel_name):
        """Add the `kernel_name` to the last layer of the code stack."""
        self._layer_kernels[-1].append((kernel_name, self._dur))

    def get_num_layers(self):
        return len(self._layers)

    def can_merge(self, other: "_CodeStack", layer_id):
        if layer_id >= self.get_num_layers() or layer_id >= other.get_num_layers():
            return False
        if layer_id >= 1:
            if not other._is_erased[layer_id - 1]:
                return False
        return self._layers[layer_id] == other._layers[layer_id]

    def mark_as_erased(self, layer_id):
        self._is_erased[layer_id] = True

    def merge(self, layer_id, other: "_CodeStack"):
        self._dur_after_merge[layer_id] = other.get_end_ts() - self._ts
        self._layer_kernels[layer_id].extend(other._layer_kernels[layer_id])
        other.mark_as_erased(layer_id)

    def get_end_ts(self):
        return self._ts + self._dur

    def _get_event(self, layer_id):
        args = {}
        kernels = sorted(self._layer_kernels[layer_id], key=lambda x: -x[1])
        if kernels:
            args.update(
                {
                    "n_kernels": len(kernels),
                    "kernels": "\n".join(
                        [f"({kernel[1]} us) {kernel[0]}" for kernel in kernels]
                    ),
                }
            )
        return {
            "args": args,
            "name": self._layers[layer_id],
            "ph": "X",
            "pid": self._pid,
            "tid": self._tid,
            "ts": self._ts,
            "dur": self._dur_after_merge[layer_id] - 0.001 * layer_id,
        }

    def get_events(self):
        return [
            self._get_event(layer_id)
            for layer_id in range(self.get_num_layers())
            if not self._is_erased[layer_id]
        ]

    def get_stack_str(self):
        return "\n".join(self._layers)

    @staticmethod
    def merge_code_stacks(code_stacks: list):
        """Merges adjacent code_stack in `code_stacks`."""
        n = len(code_stacks)
        n_layer = (
            max([cs.get_num_layers() for cs in code_stacks]) if len(code_stacks) else 0
        )
        for layer_id in range(n_layer):
            # Merge adjancent locations.
            start = 0
            while start < n:
                end = start + 1
                while end < n:
                    if code_stacks[start].can_merge(code_stacks[end], layer_id):
                        code_stacks[start].merge(layer_id, code_stacks[end])
                        end = end + 1
                    else:
                        break
                start = end


class _CodeFileTreeNode:
    def __init__(self, name="", content=None):
        self.name = name
        self.children = {}
        self.content = content
        self.heatmap = []

    def to_dict(self):
        result = {"name": self.name}
        if self.children:
            result["children"] = [child.to_dict() for _, child in self.children.items()]
        else:
            result["content"] = self.content
            result["heatmap"] = self.heatmap
        return result


class _CodeFileTree:
    """A data structure to store the modelcode."""

    def __init__(self, code_path):
        self._root = _CodeFileTreeNode()
        with tarfile.open(code_path, "r:gz") as tar:
            fnames = tar.getnames()
            for fname in fnames:
                member = tar.getmember(fname)
                if member.isfile() and member.name.endswith(".py"):
                    content = tar.extractfile(member).read().decode("utf-8")
                    self.add_file(fname, content)

    def add_file(self, file_path, content):
        parts = file_path.split(os.path.sep)
        x = self._root
        for dirname in parts[:-1]:
            if dirname not in x.children:
                x.children[dirname] = _CodeFileTreeNode(dirname)
            x = x.children[dirname]

        file_name = parts[-1]
        x.children[file_name] = _CodeFileTreeNode(file_name, content)

    def find(self, file_path):
        parts = file_path.split(os.path.sep)
        x = self._root
        for dirname in parts:
            if dirname not in x.children:
                return None
            x = x.children[dirname]
        return x

    def __str__(self):
        return json.dumps(
            [x.to_dict() for _, x in self._root.children.items()], indent=4
        )


class TimelineHelper:
    """Helper class for decoding/polishing a Torch timeline json."""

    def __init__(self, timeline_json):
        self._timeline = timeline_json
        self._events = self._timeline["traceEvents"]
        self._main_model_dir = ""
        self._kernel_pid, self._kernel_tid = self._find_gpu_stream()
        self._only_keep_1_step()
        self._add_idx()
        self._thread_indicies_sorted = self._group_by_thread()
        self._add_func_call_parent()
        self._add_flow_parent()
        self._kernel_loc, self._bwd_kernel_indices = self._get_kernel_loc()
        self._add_code_location_events()

    def export(self):
        """Returns the final timeline json (in dict)."""
        return self._timeline

    def export_code_heat_map_json(self, code_path: str):
        """Returns a json str for visualising model code heat map."""
        # runtime for 1 step (in us)
        step_runtime = 0.0
        for i, _ in self._kernel_loc:
            step_runtime += self._events[i].get("dur", 0.0)

        # count runtime for each code line
        code_line_runtime = {}
        for i, stacks in self._kernel_loc:
            is_bwd = i in self._bwd_kernel_indices
            dur = self._events[i].get("dur", 0.0)
            per_op_dur = float(dur) / float(max(1, len(stacks)))
            for stack in stacks:
                for line in set(stack):
                    if line not in code_line_runtime:
                        code_line_runtime[line] = {"fwd": 0, "bwd": 0}
                    if is_bwd:
                        code_line_runtime[line]["bwd"] += per_op_dur
                    else:
                        code_line_runtime[line]["fwd"] += per_op_dur
        code_file_tree = _CodeFileTree(code_path)
        for code_line, value in code_line_runtime.items():
            try:
                fpath, line_no = code_line.split(":")
                x = code_file_tree.find(fpath)
                fwd, bwd = value["fwd"], value["bwd"]
                heat = (fwd + bwd) / step_runtime * 100.0
                if x:
                    x.heatmap.append(
                        {
                            "line": int(line_no),
                            "heat": heat,
                            "fwd_us": fwd,
                            "bwd_us": bwd,
                        }
                    )
            except Exception as _:
                continue
        return str(code_file_tree)

    def export_flame_json(self):
        """Returns a json str for visualising flame chart."""
        code_stacks = []
        for i, stacks in self._kernel_loc:
            kernel_dur = self._events[i]["dur"]
            per_op_dur = float(kernel_dur) / float(max(1, len(stacks)))
            for stack in stacks:
                cs = _CodeStack(stack, 0, 0, 0, per_op_dur)
                cs.set_leaf_kernel(self._events[i].get("name", ""))
                code_stacks.append(cs)
        code_stacks = sorted(code_stacks, key=lambda cs: cs.get_stack_str())
        now_ts = 0
        for code_stack in code_stacks:
            code_stack._ts = now_ts
            now_ts += code_stack._dur
        _CodeStack.merge_code_stacks(code_stacks)
        f_events = []
        for code_stack in code_stacks:
            f_events.extend(code_stack.get_events())
        flame_json = {
            "displayTimeUnit": "ns",
            "metadata": {"highres-ticks": True},
            "traceEvents": f_events,
        }
        return json.dumps(flame_json, indent=4)

    def _add_idx(self):
        """Adds IDX=i to self._events[i]'s args."""
        for i, event in enumerate(self._events):
            _set_event_arg(event, "IDX", i)

    def _only_keep_1_step(self):
        """Removes all events after the 1st profilng step."""
        step_start_ts = [1e50, 1e50]
        for event in self._events:
            pid, tid = event.get("pid", -1), event.get("tid", -1)
            if (pid, tid) != (self._kernel_pid, self._kernel_tid):
                continue
            if event.get("cat", "") != "gpu_user_annotation":
                continue
            if "ProfilerStep" in event.get("name", ""):
                step_start_ts.append(event.get("ts", 0))
        max_ts = sorted(step_start_ts)[1]
        new_events = [e for e in self._events if e.get("ts", 0) < max_ts]
        self._events.clear()
        self._events.extend(new_events)

    def _group_by_thread(self):
        thread_indicies = {}
        for event in self._events:
            pid, tid = event.get("pid", -1), event.get("tid", -1)
            if not (pid != -1 and tid != -1 and event.get("ph", "") == "X"):
                continue
            if (pid, tid) not in thread_indicies:
                thread_indicies[(pid, tid)] = []
            thread_indicies[(pid, tid)].append(event["args"]["IDX"])
        for pid_tid in thread_indicies:
            thread_indicies[pid_tid] = sorted(
                thread_indicies[pid_tid], key=lambda i: self._events[i]["ts"]
            )
        return thread_indicies

    def _add_func_call_parent(self):
        """Adds P=j to events[i]'s args if events[j] is the caller of events[i]."""
        events = self._events
        for _, indicies in self._thread_indicies_sorted.items():
            stack = [(-1, -1e50, 1e50)]
            for idx in indicies:
                while stack[-1][0] != -1 and events[idx]["ts"] > stack[-1][2]:
                    stack.pop()
                _set_event_arg(events[idx], "P", stack[-1][0])
                stack.append(
                    (idx, events[idx]["ts"], events[idx]["ts"] + events[idx]["dur"])
                )

    def _add_flow_parent(self):
        """Adds P=j to events[i]'s args for each flow edge (events[i] -> events[j])."""
        # it maps from hash(pid, tid, ts) to IDX
        hash_to_idx = {}
        for event in self._events:
            if event.get("ph", "") == "X":
                hash = _hash(event)
                hash_to_idx[hash] = event["args"]["IDX"]

        flow_start = {}
        flow_end = {}
        for event in self._events:
            hash = _hash(event)
            if hash not in hash_to_idx:
                continue
            if event.get("ph", "") == "s":
                flow_start[event["id"]] = hash_to_idx[hash]
            if event.get("ph", "") == "f":
                flow_end[event["id"]] = hash_to_idx[hash]

        for flow_id, end_idx in flow_end.items():
            if flow_id not in flow_start:
                continue
            start_idx = flow_start[flow_id]
            _set_event_arg(self._events[end_idx], "P", start_idx)
            _set_event_arg(self._events[end_idx], "flow_jump", 1)

    def _find_gpu_stream(self):
        """Returns the (pid, tid) to the major gpu kernel thread with
        the LARGEST number of kernels."""
        gpu_pids = set()
        for event in self._events:
            if event.get("ph", "") != "M" or event.get("name", "") != "process_labels":
                continue
            process_label = event.get("args", {}).get("labels", "")
            if "GPU" in process_label or "MLU" in process_label:
                gpu_pids.add(event["pid"])
        n_kernels = {(-1, -1): 0}
        for event in self._events:
            pid, tid = event.get("pid", -1), event.get("tid", -1)
            if (pid not in gpu_pids) or (event.get("cat", "") != "kernel"):
                continue
            n_kernels[(pid, tid)] = n_kernels.get((pid, tid), 0) + 1

        (pid, tid) = sorted(n_kernels.items(), key=lambda x: x[1])[-1][0]
        return (pid, tid)

    def _simplify_fname(self, fname: str):
        """Converts `File \"/a/b/c/main_model.py\"` to `main_model.py`."""
        fname = fname.strip().replace('File "', "").replace('"', "")
        if len(self._main_model_dir) == 0 and fname.endswith("main_model.py"):
            self._main_model_dir = fname[: -len("main_model.py")]
        if fname.startswith(self._main_model_dir):
            fname = fname[len(self._main_model_dir) :]
        return fname

    def _get_compile_kernel_stacks(self, idx):
        """Returns (stacks, is_bwd) for a kernel (in compile scope)."""
        stacks = []
        is_bwd = False
        events = self._events
        stack_traces = _get_arg(events[idx], "stack", {}).get("stack_traces", [])
        for stack_trace in stack_traces:
            lines = stack_trace.split("\n")
            stack = []
            for i, line in enumerate(lines):
                # skip line for code line content
                if i % 2 == 1:
                    continue
                parts = line.split(",")
                if len(parts) != 3:
                    continue
                fname, line_no, _ = parts
                fname = self._simplify_fname(fname)
                line_no = line_no.strip().replace("line ", "")
                stack.append(f"{fname}:{line_no}")
            stacks.append(stack)
        x = _get_arg(events[idx], "P", -1)
        while x != -1:
            if events[x].get("name", "") == "CompiledFunctionBackward":
                is_bwd = True
                break
            x = _get_arg(events[x], "P", -1)
        return stacks, is_bwd

    def _get_non_compile_kernel_stacks(self, idx):
        """Returns (stacks, is_bwd) for a kernel (not in compile scope)."""
        events = self._events
        n_flow_jump = _get_arg(events[idx], "flow_jump", 0)
        x = _get_arg(events[idx], "P", -1)
        stack = []
        while x != -1:
            call_from = _get_arg(events[x], "CallFrom")
            if call_from:
                stack.append(call_from)
            n_flow_jump += _get_arg(events[x], "flow_jump", 0)
            x = _get_arg(events[x], "P", -1)
        if not stack:
            stack = ["/"]
        stacks = [stack[::-1]]
        is_bwd = n_flow_jump >= 2
        return stacks, is_bwd

    def _get_kernel_loc(self):
        """Returns (locs, bwd_kernel_indices), where
        - locs is a ordered-list where each element is of form (idx, stacks).
            - stacks is a list of stack(s)
        - bwd_kernel_indices is a set containing all bwd kernels' indices."""
        sorted_indices = self._thread_indicies_sorted.get(
            (self._kernel_pid, self._kernel_tid), []
        )
        events = self._events
        locs, bwd_kernel_indices = [], set()
        for idx in sorted_indices:
            if not _is_kernel(events[idx]):
                continue
            stacks = []
            if "stack_traces" in _get_arg(events[idx], "stack", {}):
                stacks, is_bwd = self._get_compile_kernel_stacks(idx)
            else:
                stacks, is_bwd = self._get_non_compile_kernel_stacks(idx)
            locs.append((idx, stacks))
            if is_bwd:
                bwd_kernel_indices.add(idx)
        return locs, bwd_kernel_indices

    def _add_code_location_events(self):
        """Adds code locations for all gpu kernels."""
        loc_tid = 1000000
        self._events.append(
            {
                "name": "thread_name",
                "ph": "M",
                "pid": self._kernel_pid,
                "tid": loc_tid,
                "args": {"name": "Code Location"},
            }
        )

        # Get original location for each kernel.
        code_stacks = []
        for i, stacks in self._kernel_loc:
            start = self._events[i]["ts"]
            kernel_dur = self._events[i]["dur"]
            n_op = max(1, len(stacks))
            per_op_dur = float(kernel_dur) / float(n_op)
            for j, stack in enumerate(stacks):
                code_stacks.append(
                    _CodeStack(
                        stack,
                        self._kernel_pid,
                        loc_tid,
                        start + j * per_op_dur,
                        per_op_dur,
                    )
                )
        _CodeStack.merge_code_stacks(code_stacks)
        for cs in code_stacks:
            self._events.extend(cs.get_events())

    def merge_parent(self, id, depth, name=""):
        if depth == 0:
            return name
        name = self._events[id]["name"] + "::" + name
        parent = self._events[id]["args"]["P"]
        return self.merge_parent(parent, depth - 1, name)

    def analysis_leaf(self, leaf_name, parent_depth):
        """
        output dict: leaf-subleaf: runtime, parent
        """
        ret = {}
        for event in self._events:
            if event["name"] == leaf_name:
                parent_merge_name = self.merge_parent(event["args"]["P"], parent_depth, leaf_name)
                if parent_merge_name not in ret:
                    ret[parent_merge_name] = {"runtime": event["dur"], "count": 1}
                else:
                    ret[parent_merge_name]["runtime"] += event["dur"]
                    ret[parent_merge_name]["count"] += 1
        return ret


if __name__ == "__main__":
    timeline_h20json = json.load(open("./h20simple.json"))
    timeline_h20helper = TimelineHelper(timeline_h20json)
    timeline_hwjjson = json.load(open("./mlusimple.json"))
    timeline_hwjhelper = TimelineHelper(timeline_hwjjson)
    d1=timeline_h20helper.analysis_leaf("Memcpy DtoD (Device -> Device)", 3)
    d2=timeline_h20helper.analysis_leaf("void at::native::elementwise_kernel<128, 2, at::native::gpu_kernel_impl_nocast<at::native::direct_copy_kernel_cuda(at::TensorIteratorBase&)::{lambda()#3}::operator()() const::{lambda()#7}::operator()() const::{lambda(float)#1}>(at::TensorIteratorBase&, at::native::direct_copy_kernel_cuda(at::TensorIteratorBase&)::{lambda()#3}::operator()() const::{lambda()#7}::operator()() const::{lambda(float)#1} const&)::{lambda(int)#1}>(int, at::native::gpu_kernel_impl_nocast<at::native::direct_copy_kernel_cuda(at::TensorIteratorBase&)::{lambda()#3}::operator()() const::{lambda()#7}::operator()() const::{lambda(float)#1}>(at::TensorIteratorBase&, at::native::direct_copy_kernel_cuda(at::TensorIteratorBase&)::{lambda()#3}::operator()() const::{lambda()#7}::operator()() const::{lambda(float)#1} const&)::{lambda(int)#1})", 3)
    d3=timeline_hwjhelper.analysis_leaf("void torch_mlu::ops::mluUnion1StridedSliceNCLargeC<int>(void const*, void*, bool, int, int, int, int, int)", 3)
    print(d1)
    print(d2)
    print(d3)
