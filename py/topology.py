import os
import glob
import socket
import subprocess
import logging
from typing import Dict, List, Optional

"""
To find out below answer:
on current proc:

  visible sockets - visible nic - visible cpu/numa - gpu affinity

We get all topology then find visible part

return dict:
  socket 1 - numa 0 - GPU 0
           - numa 1 - GPU 1
  socket 2 - numa 2 - GPU 2
           - numa 3 - GPU 3
and list of nic:
  socket -1 - [nics]
  socket 1 - [nics]
  socket 2 - [nics]
"""

_NODE_PATH = "/sys/devices/system/node/{}/cpulist"
_CPU_PATH = "/sys/devices/system/"
_PACKAGE_ID = "/topology/physical_package_id"
_PCIE_PATH = "/sys/bus/pci/devices/"
_PROC_PATH = "/proc/self/status"
_NET_PATH = "/sys/class/net"
_GPU_CLASS_CODE = ["0x030200"]

class Topology:
    """
    Full topology for this machine
    """
    def __init__(self):
        # get all status first
        cpu_infos = get_all_cpu_socket_numa()
        self.gpu_infos = get_all_gpu_with_numa()
        self.nic_infos = get_all_up_nics_with_numa()
        # get all visible
        visible_cpus, visible_numa = read_proc_status()
        self.visible_gpu = get_visible_gpu_with_numa()
        self.numa_infos = {}
        self.visible_socket_numa = {}
        for _, socket_numa in cpu_infos.items():
            self.numa_infos[socket_numa[1]] = socket_numa[0]
        for cpu in visible_cpus:
            socket_numa = cpu_infos[cpu]
            if socket_numa[0] not in self.visible_socket_numa:
                self.visible_socket_numa[socket_numa[0]] = []
            if socket_numa[1] in visible_numa:
                self.visible_socket_numa[socket_numa[0]].append(socket_numa[1])
            else:
                logging.warn("found numa {} not in visible mem but used by visible cpu{}".format(socket_numa[1], cpu))
        for socket, numas in self.visible_socket_numa.items():
            self.visible_socket_numa[socket] = sorted(list(set(numas)))

    def __str__(self):
        info = ""
        info += "================= CPU INFO ==================\n"
        info += "================= All NUMA ==================\n"
        for numa, socket in self.numa_infos.items():
            info += "NUMA id: {}, Socket id: {}\n".format(numa, socket)
        info += "============== VISIBLE SOCKET ===============\n"
        for socket, numas in self.visible_socket_numa.items():
            info += "Socket id: {}, visible numas: {}\n".format(socket, numas)
        info += "================= GPU INFO ==================\n"
        info += "================= All GPUs ==================\n"
        for gpu in self.gpu_infos:
            info += str(gpu) + " socket id: {}\n".format(self.numa_infos[gpu.numa])
        info += "================ VISIBLE GPU ==================\n"
        for idx, gpu in enumerate(self.visible_gpu):
            info += "Idx: {}".format(idx) + str(gpu) + " socket id: {}\n".format(self.numa_infos[gpu.numa])
        info += "================= NIC INFO ==================\n"
        for nic in self.nic_infos:
            info += str(nic) + " socket id: {}\n".format(self.numa_infos[nic.numa])
        return info

def read_proc_status() -> List[int], List[int]:
    """
    Get Cpu allow list and mem allow list from proc status
    """
    _CPU_KEY = "Cpus_allowed_list:"
    _NUMA_KEY = "Mems_allowed_list:"
    def parse_list(keyword, line):
        # parse keyword:	0-3,16-19
        ret = []
        set_str = line.split(":", 1)[1].strip()
        if set_str and set_str != "":
            for part in set_str.split(","):
                if "-" in part:
                    start, end = part.split("-")
                    ret.extend(range(int(start), int(end) + 1))
                else:
                    ret.append(int(part))
        return ret

    cpu_list = []
    numa_list = []
    if os.path.exists(_PROC_PATH):
        try:
            with open(_PROC_PATH, "r") as f:
                for line in f:
                    if line.startswith(_CPU_KEY):
                        cpu_list.extend(parse_list(_CPU_KEY))
                        continue
                    if line.startswith(_NUMA_KEY):
                        numa_list.extend(parse_list(_NUMA_KEY))
                        continue
            # sort
            cpu_list = sorted(list(set(cpu_list)))
            numa_list = sorted(list(set(numa_list)))
        except Exception as e:
            logging.error("read /proc/self/status failed: {}".format(e))
            raise Exception("Read proc status failed")
    else:
        logging.error("read /proc/self/status failed: file not existed")
        raise Exception("Read proc status failed")

    return cpu_list, numa_list

def get_all_cpu_socket_numa() -> Dict[int, List[int]]:
    """
    return {cpu_id,[socket_id, numa_id]}
    """
    cpu_info_map = {}
    if not os.path.exists(_CPU_PATH):
        logging.error("read cpu status failed.")
        raise Exception("No cpu status info.")
    for cpu_dir in os.listdir(_CPU_PATH):
        if not cpu_dir.startswith("cpu") or not cpu_dir[3:].isdigit():
            continue
        cpu_id = int(cpu_dir[3:])
        topo_path = os.path.join(_CPU_PATH, cpu_dir, "topology")
        socket_path = os.path.join(topo_path, "physical_package_id")
        socket_id = -1
        if os.path.exists(socket_path):
            try:
                with open(socket_path, "r") as f:
                    socket_id = int(f.read().strip())
            except Exception as e:
                logging.error("Bad socket id for {}: {}".format(cpu_id, e))
                raise Exception("Read proc status failed")
        else:
            logging.error("Bad socket id for {}: no socket file".format(cpu_id))
            raise Exception("Read proc status failed")
        if socket_id == -1:
            logging.error("Bad socket id for {}".format(cpu_id))
            raise Exception("Read proc status failed")
        cpu_info_map[cpu_id] = [socket_id]
        numa_path = os.path.join(topo_path, "numa_node")
        numa_id = -1
        try:
            numa_id = int(numa_path.read().strip())
        except Exception:
            raise Exception("Read proc numa status failed")
        cpu_info_map[cpu_id].append(numa_id)
    return cpu_info_map

class Nic:
    def __init__(self, type_name, iface, ips, numa):
        if len(ips) == 0:
            raise Exception("init nic with empty ip")
        self.type_name = type_name
        self.iface = iface
        self.ips = ips
        self.numa = numa

    def __eq__(self, other):
        if not isinstance(other, Nic):
            raise Exception("comp nic failed")
        return self.ips[0] == other.ips[0]

    def __lt__(self, other):
        if not isinstance(other, Nic):
            raise Exception("sort nic failed")
        return self.ips[0] < other.ips[0]

    def __str__(self):
        info = "Nic: type {},  iface {},  ips {},  numa {}".format(self.type_name, self.iface, self.ips, self.numa)
        return info


def IsIpv4(addr):
    try:
        socket.inet_aton(addr)
        return True
    except socket.error:
        return False


def IsIpv6(addr):
    try:
        socket.inet_pton(socket.AF_INET6, addr)
        return True
    except socket.error:
        return False


def GetIpByIface(iface):
    # whitelist for ipv6
    ByteDance_ipv6_default_prefix = [
        "fdbd:",
        "2605:340:CD",
    ]
    # blacklist for ipv4
    invalid_ipv4_prefix = [
        "22.",
    ]
    avail_ips = []
    # get IPv4 addr
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        packeted_if = struct.pack("256s", iface[:15].encode("utf-8"))
        packed_ip = fcntl.ioctl(s.fileno(), 0x8915, packeted_if)[20:24]  # SIOCGIFADDR
        ip_addr = socket.inet_ntoa(packed_ip[:4])
        if not any(ip_addr.startswith(prefix) for prefix in invalid_ipv4_prefix):
            avail_ips.append(ip_addr)
        else:
            logging.info(
                "{}: IPv4 address {} ignored due to invalid prefix".format(
                    iface, ip_addr
                )
            )
    except Exception as e:
        logging.warning("get IPv4 of iface[{}] failed, msg[{}]".format(iface, e))
    # get IPv6 addr
    try:
        # get ipv6 by ip -6 addr show <iface>
        ipv6 = re.search(
            re.compile(r"(?<=inet6 )(.*)(?=\/)", re.M),
            os.popen("ip -6 addr show {}".format(iface)).read(),
        )
        if ipv6 is not None:
            ip_addr = ipv6.groups()[0]
            for prefix in ByteDance_ipv6_default_prefix:
                if ip_addr.lower().startswith(prefix.lower()):
                    avail_ips.append(ip_addr)
                    return avail_ips
            logging.warning("No proper IPv6 address found")
        else:
            logging.warning("Given netcard name not found in ip addr infos")
    except Exception as e:
        logging.warning("get IPv6 of iface[{}] failed, msg[{}]".format(iface, e))
    logging.error("Failed to find IPv6 address")
    return avail_ips

def get_all_up_nics_with_numa() -> List[Nic]:
    nic_type_rules = {
        "bond": re.compile(r'^bond\d+$'),  # bond0, bond1
        "carma": re.compile(r'^carma\d+$'), # carma0, carma1
        "eth": re.compile(r'^(eth\d+|enp\d+|ens\d+|eno\d+)$')  # eth
    }
    if not os.path.exists(_NET_PATH):
        logging.error("Can not get nic info")
        raise Exception("Found no /sys/class/net")
    ret = []
    for nic in os.listdir(net_path):
        # iface
        if nic == "lo":
            continue
        nic_path = os.path.join(_NET_PATH, nic)
        if not os.path.isdir(nic_path):
            continue
        # type_name
        nic_type = None
        for type_name, pattern in nic_type_rules.items():
            if pattern.match(nic):
                nic_type = type_name
                break
        if not nic_type:
            continue
        # state
        link_state = "down"
        operstate_path = os.path.join(nic_path, "operstate")
        if os.path.exists(operstate_path):
            try:
                link_state = open(operstate_path, "r").readline().strip()
            except:
                logging.info("No nic stat, use down")
        if link_state != "up":
            continue
        # numa
        numa = -1
        numa_path = os.path.join(nic_path, "device/numa_mode")
        if os.path.exists(numa_path):
            try:
                numa = int(open(numa_path, "r").readline().strip())
            except:
                logging.warning("can not read numa for {}".format(nic))
        # ip
        ips = GetIpByIface(nic)
        ret.append(Nic(nic_type, nic, ips, numa))
    return sorted(ret)

class GPU:
    def __init__(self, bdf, numa_id):
        self.numa = numa_id
        self.bdf = bdf

    def __eq__(self, other):
        if not isinstance(other, GPU):
            raise Exception("comp GPU failed")
        return self.bdf == other.bdf

    def __lt__(self, other):
        if not isinstance(other, GPU):
            raise Exception("sort GPU failed")
        return self.bdf < other.bdf

    def __str__(self):
        info = "GPU: bdf_id {},  numa_id {}".format(self.bdf, self.numa)
        return info


def get_visible_gpu_with_numa() -> List[GPU]:
    visible_gpu_info = []
    try:
        nvidia_gpu_infos = (
            os.popen('nvidia-smi -x -q |grep "gpu id"').read().strip().split("\n")
        )
        for gpu_info in nvidia_gpu_infos:
            bdf_id = gpu_info.split('"')[1].split(":")
            bdf_id = "0000:{}:{}".format(bdf_id[1].lower(), bdf_id[2].lower())
            numa_path = os.path.join(_PCIE_PATH, bdf_id + "/numa_node")
            numa = int(open(numa_path, "r").readline().strip())
            visible_gpu_info.append(GPU(bdf_id, numa))

    except Exception as e:
        logging.error("Get Gpu Numa Info failed since {}".format(e))
        raise Exception("Read visible gpu status failed")
    return visible_gpu_info

def get_all_gpu_with_numa() -> List[GPU]:
    gpu_info = []
    if not os.path.exists(_PCIE_PATH):
        logging.warning("Cannot access {}".format(_PCIE_PATH))
        return gpu_info
    for bdf in os.listdir(_PCIE_PATH):
        dev_path = os.path.join(_PCIE_PATH, bdf)
        if not os.path.isdir(dev_path):
            continue
        class_path = os.path.join(dev_path, "class")
        if not os.path.exists(class_path):
            continue
        try:
            class_code = open(class_path, "r").readline().strip()
        except:
            continue

        if class_code not in _GPU_CLASS_CODE:
            continue

        numa_node = -1
        numa_path = os.path.join(dev_path, "numa_mode")
        if os.path.exists(numa_path):
            try:
                numa = int(open(numa_path, "r").readline().strip())
            except:
                logging.warning("can not read numa for {}".format(bdf))
        gpu_info.append(GPU(bdf, numa_node))
    return sorted(gpu_info)


if __name__ == "__main__":
    topo = Topology()
    print(topo)
