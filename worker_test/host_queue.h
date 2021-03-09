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
 * Created Time:Mon 20 Jul 2020 04:26:26 AM EDT
 * File Name:./host_queue.h
 * Description:
*************************************************************************/
#ifndef MAGICMIND_RUNTIME_RESOURCE_HOST_QUEUE_H_
#define MAGICMIND_RUNTIME_RESOURCE_HOST_QUEUE_H_
#include "magicmind/runtime/resource/queue_interface.h"
namespace magicmind {

// Just treat this as an one-worker thread pool.
class HostQueue : public Queue {
 public:
  // CPU Only
  // For more info, see mlu_queue.h
  explicit HostQueue(DeviceHandle *device_handle) : Queue(device_handle) {}
  ~HostQueue() override;
  void Enqueue(std::function<void(KernelResource *)> task, KernelResource *res) override;
  void Wait(Sptr<Event> event) override;
  void Notify(Sptr<Event> event) override;
  void Sync() override;
};
}  // namespace magicmind
#endif  // MAGICMIND_RUNTIME_RESOURCE_HOST_QUEUE_H_
