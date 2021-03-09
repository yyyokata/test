/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:Tue 09 Mar 2021 01:40:31 PM CST
 * File Name:test4.cpp
 * Description: 
*************************************************************************/
#include <memory>
#include <queue>
#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <iostream>
#include <atomic>
#include <thread>

class A {
 public:
  A() {
    c_.store(0, std::memory_order_relaxed);
  }
  void atomic_s() {
    c_--;
  }
  void call_back(std::function<void()> *f) {
    *f = [this](){c_--;};
  }
  void print() {
    std::cout << c_;
  }
 private:
  std::atomic<int> c_;
};


int main() {
  auto a = new A();
  a->atomic_s();
  a->print();
  std::function<void()> f;
  a->call_back(&f);
  f();
  a->print();
  return 0;
}
