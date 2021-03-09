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
 * Created Time:Mon 20 Jul 2020 05:00:59 AM EDT
 * File Name:./mlu_queue.h
 * Description:
*************************************************************************/
#ifndef MAGICMIND_RUNTIME_RESOURCE_MLU_QUEUE_H_
#define MAGICMIND_RUNTIME_RESOURCE_MLU_QUEUE_H_

#include "magicmind/runtime/resource/queue_interface.h"
namespace magicmind {
class MLUQueue : public Queue {
 public:
  explicit MLUQueue(DeviceHandle *dev);
  ~MLUQueue() override;
  // MLU queue should put itself into res before enqueue
  void Enqueue(std::function<void(KernelResource *)> task, KernelResource *res) override;
  void Wait(Sptr<Event> event) override;
  void Notify(Sptr<Event> event) override;
  // The problem about sync is that we must very sure about
  // EACH task we want to push into queue has been pushed
  // because Execute() is however an unblocking function.
  // To avoid this problem, one mlu queue is actually
  // an mlu+host queue.
  void Sync() override;

 private:
  // A task function means:
  // task (KernelResource) {
  //   Get some resource from res;
  //   Queue = getQueue from res;
  //   cnrtInvoke(Queue, resource);
  // }
  // workload between execute and invoke should not be heavy
  // or this execute func itself will seems like a bottleneck
  cnrtQueue_t mlu_queue() const;

 private:
  DeviceHandle *device_handle_;
  cnrtQueue_t mlu_queue_ = nullptr;
  cnrtDev_t dev_;
};

}  // namespace magicmind
#endif  // MAGICMIND_RUNTIME_RESOURCE_MLU_QUEUE_H_
