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
 * Created Time:Mon 20 Jul 2020 05:04:49 AM EDT
 * File Name:./mlu_queue.cc
 * Description:
*************************************************************************/
#include <cstdio>
#include <utility>
#include "magicmind/runtime/resource/mlu_queue.h"
#include "magicmind/runtime/resource/hack_tools.h"
namespace magicmind {
// TODO(LIN): complete error ret
// TODO(LIN): complete log
// Memo: 1 important thing about queue:
// Members created inside task can not be accessed outside safe without a sync.
MLUQueue::MLUQueue(DeviceHandle *dev) : Queue(dev) {
  cnrtDev_t ori_dev;
  cnrtGetCurrentDevice(&ori_dev);
  cnrtGetDeviceHandle(&dev_, static_cast<uint64_t>(dev->index()));
  cnrtSetCurrentDevice(dev_);
  cnrtCreateQueue(&mlu_queue_);
  cnrtSetCurrentDevice(ori_dev);
  EnqueueWithNoParam([this]() {
    VLOG(MID) << DebugString() << "MLUQueue init task";
    cnrtSetCurrentDevice(dev_);
    VLOG(MID) << DebugString() << "MLUQueue init done";
  });
}

// destory both thread and cnrt_queue;
MLUQueue::~MLUQueue() {
  VLOG(MID) << DebugString() << "MLUQueue final job";
  EnqueueWithNoParam(nullptr);
  VLOG(MID) << DebugString() << "MLUQueue destroy";
  JoinThread();
  cnrtDestroyQueue(mlu_queue_);
  mlu_queue_ = nullptr;
  // thread gone, block till done
  // detach or join?
  VLOG(MID) << DebugString() << "destroy queue";
  // TODO(LIN): clear queue?
}

void MLUQueue::Enqueue(std::function<void(KernelResource *)> task, KernelResource *res) {
  VLOG(MID) << DebugString() << "Enqueue";
  res->SetQueue(mlu_queue());
  EnqueueWithNoParam(std::bind(std::move(task), res));
}

// TODO(LIN) : could be better
void MLUQueue::Wait(Sptr<Event> event) {
  event->SetReceiver(GetMLUQueueFromCNRTQueue(mlu_queue()));
  VLOG(MID) << DebugString() << "MLUQueue wait task";
  EnqueueWithNoParam([this, event]() { event->Wait(); });
}

void MLUQueue::Notify(Sptr<Event> event) {
  event->SetSender(GetMLUQueueFromCNRTQueue(mlu_queue()));
  VLOG(MID) << DebugString() << "MLUQueue notify task";
  EnqueueWithNoParam([this, event]() { event->Notify(); });
}

// 1. Send an sync to queue.
// 2. Make sure sync is done.
void MLUQueue::Sync() {
  auto event_ = EventFactory::Global()->Create();
  VLOG(MID) << DebugString() << "push an event in";
  EnqueueWithNoParam([&event_, this]() {
    cnrtSyncQueue(mlu_queue());
    event_->Notify();
  });
  event_->Wait();
  VLOG(MID) << DebugString() << "catch event";
}

cnrtQueue_t MLUQueue::mlu_queue() const {
  return mlu_queue_;
}

}  // namespace magicmind
