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

bool test() {
  static int a = 4;
  std::cout << std::to_string(++a);
  return a > 3;
}

bool test2(const int &a) {
  return a < 3;
}

bool test3(const float &a) {
  return a > 4;
}

template <class T>
bool default_checker(const T &value) {
  return true;
}

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

class B {
 public:
  B(std::function<void()> f) {
    f_ = std::move(f);
  }
  void ha() {
    f_();
  }
 private:
  std::function<void()> f_;
};

template <typename T>
using Checker = bool (*)(const T &);

template <typename T, Checker<T> checker = default_checker<T>>
class C {
 public:
  bool test(const T &value) {
    return checker(value);
  }
};

int main() {
  auto a = new A();
  a->atomic_s();
  a->print();
  std::function<void()> f;
  a->call_back(&f);
  f();
  a->print();
  auto b = B(test);
  b.ha();
  test();
  auto c1 = C<int>();
  auto c2 = C<int, test2>();
  auto c3 = C<float, test3>();
  std::cout << c1.test(3);
  std::cout << c2.test(3);
  std::cout << c3.test(3);
  return 0;
}
