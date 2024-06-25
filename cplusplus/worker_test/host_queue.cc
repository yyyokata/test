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
 * Created Time:Mon 20 Jul 2020 04:40:24 AM EDT
 * File Name:host_queue.cc
 * Description:
*************************************************************************/
#include <memory>
#include <utility>
#include "magicmind/runtime/resource/host_queue.h"
namespace magicmind {

HostQueue::~HostQueue() {
  // lock
  EnqueueWithNoParam(nullptr);
  // thread gone, block till done
  // detach or join?
  VLOG(MID) << DebugString() << "destroy queue";
  JoinThread();
  VLOG(MID) << DebugString() << "done";
}

void HostQueue::Enqueue(std::function<void(KernelResource *)> task, KernelResource *res) {
  VLOG(MID) << DebugString() << "Do execute";
  EnqueueWithNoParam(std::bind(std::move(task), res));
  VLOG(MID) << DebugString() << "push done";
}

void HostQueue::Sync() {
  auto event_ = EventFactory::Global()->Create();
  VLOG(MID) << DebugString() << "push an event in";
  EnqueueWithNoParam([&event_]() { event_->Notify(); });
  event_->Wait();
  VLOG(MID) << DebugString() << "catch event";
}

void HostQueue::Wait(Sptr<Event> event) {
  EnqueueWithNoParam([event]() { event->Wait(); });
}

void HostQueue::Notify(Sptr<Event> event) {
  EnqueueWithNoParam([event]() { event->Notify(); });
}

}  // namespace magicmind
