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
 * Created Time:Mon 20 Jul 2020 04:43:27 AM EDT
 * File Name:./queue_interface.h
 * Description:
*************************************************************************/
#include <memory>
#include <queue>
#include <vector>
#include <utility>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>

class monitor {
 public:
  void AddInput(int num) {
    buffer.push_back(num);
    if (buffer.size() % 4 == 0) {
      pool_.Enqueue(std::bind(callback, std::vector<int>(buffer.begin(), buffer.begin()+4); //监视可以同步也可以异步
    }
  }
  void JoinAll() {
    pool_.JoinAll();//同步式返回所有结果。
  }
  //另一种做法
  void RegCallback(std::function<void(std::vector<int>)> cb) {
    callback = [] (std::vector<int> input, std::condition_variable cv) {
      cb(input);
      cv_.notify_all(); //信号量的另一段放在主线程，就绪时再去拷贝。
    };
  }
  /////
 private:
  std::vector<int> buffer;
  ThreadPool pool_;
  std::function<void(std::vector<int>)> callback;
}

class ThreadPool {
 public:
  explicit ThreadPool(int thread_num) {
    for (int count = 0; count < thread_num; ++count) {
      execute_threads_.push_back(std::thread([this]() {DoWork();}));
    }
  }
  ~ThreadPool() {
  }
  // Basic enqueue/sync interface.
  void Enqueue(std::function<void()> task) {
    {
      std::unique_lock<std::mutex> lk(mtx_);
      function_queue_.push(std::move(fn));
    }
    cv_.notify_all();
  }
  void JoinAll() {
    for (auto t_ : execute_threads_) {
      t_.join();
    }
  }
  bool IsQueueEmpty() {
    return function_queue_.empty();
  }

 protected:
  void DoWork() {
    while (true) {
      std::function<void()> fn;
      // Atomic
      {
        std::unique_lock<std::mutex> lk(mtx_);
        // not quite sure about this idle flag
        cv_.wait(lk, [this]() { return !IsQueueEmpty(); });
        fn = std::move(function_queue_.front());
        function_queue_.pop();
      }
      if (!fn) {
        // push nullptr to return;
        return;
      }
      fn();
    }
  }

 private:
  // TODO(LIN): dev handle
  // Dependency control
  std::mutex mtx_;
  std::condition_variable cv_;
  // function queue
  std::queue<std::function<void()>> function_queue_;
  // Worker thread
  std::vector<std::thread> execute_threads_;
};

