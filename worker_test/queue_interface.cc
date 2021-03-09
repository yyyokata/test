/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *************************************************************************/
/*************************************************************************
 * Author: Nan Lin
 * Created Time:Tue 21 Jul 2020 07:05:01 AM EDT
 * File Name:./queue_interface.cc
 * Description:
*************************************************************************/
#include <string>
#include "magicmind/runtime/resource/queue_interface.h"
#include "magicmind/runtime/resource/hack_tools.h"
#include "magicmind/runtime/resource/mlu_queue.h"
#include "magicmind/runtime/resource/host_queue.h"
#include "magicmind/util/types.h"

namespace magicmind {

Queue::Queue(DeviceHandle *dev) : device_handle_(dev) {
  execute_thread_ = absl::make_unique<std::thread>([this]() { DoWork(); });
}

bool Queue::IsQueueEmpty() {
  return function_queue_.empty();
}

void Queue::DoWork() {
  while (true) {
    std::function<void()> fn;
    // Atomic
    {
      std::unique_lock<std::mutex> lk(mtx_);
      VLOG(BOT) << DebugString() << "waiting for queue";
      // not quite sure about this idle flag
      cv_.wait(lk, [this]() { return !IsQueueEmpty(); });
      fn = std::move(function_queue_.front());
      function_queue_.pop();
    }
    if (!fn) {
      // push nullptr to return;
      VLOG(BOT) << DebugString() << "finish!";
      return;
    }
    fn();
  }
}

// A private version of execute
// Allow function without param to be pushed into queue.
void Queue::EnqueueWithNoParam(std::function<void()> fn) {
  VLOG(MID) << DebugString() << "push a fn into queue";
  // Atomic
  {
    std::unique_lock<std::mutex> lk(mtx_);
    function_queue_.push(std::move(fn));
  }
  cv_.notify_all();
  VLOG(MID) << DebugString() << "push done";
}

// Do not use it excepted destroy func
void Queue::JoinThread() {
  execute_thread_->join();
}

DeviceHandle *Queue::GetDeviceHandle() const {
  return device_handle_;
}

void Queue::SetName(std::string name) {
  name_ = name;
}

std::string Queue::DebugString() const {
  return device_handle_->DebugString() + " " + name_;
}

QueueMgr::QueueMgr(DeviceHandle *dev, uint32_t queue_num) : dev_(dev), queue_num_(queue_num) {
  if (!dev) {
    LOG(ERROR) << "Not an invalid device.";
  }
  if (dev->GetDeviceType().IsCpuDevice()) {
    queue_factory_ = [](DeviceHandle *dev) -> Queue * { return new HostQueue(dev); };
  } else {
    queue_factory_ = [](DeviceHandle *dev) -> Queue * { return new MLUQueue(dev); };
  }
  queues_.reserve(queue_num * 2);
  Extend(queue_num);
}

QueueMgr::~QueueMgr() {
  for (const auto &e : queues_) {
    delete e;
  }
}

Queue *QueueMgr::GetQueue() {
  return GetQueue(0);
}

Queue *QueueMgr::GetQueue(uint32_t index) {
  if (index >= queues_.size() || !queues_[index]) {
    LOG(ERROR) << "There is no available queue in" << dev_->DebugString() << " index:" << index;
    return nullptr;
  }
  return queues_[index];
}

void QueueMgr::Extend(uint32_t queue_num) {
  for (int i = 0; i < queue_num; ++i) {
    queues_.emplace_back(queue_factory_(dev_));
    int queue_id = queues_.size() - 1;
    queues_[queue_id]->SetName(std::to_string(queue_id) + ":");
  }
}

}  // namespace magicmind
