/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <type_traits>
#include <typeinfo>
#include <iostream>
#include <vector>
#include "./demangle.hpp"

template <typename T>
class test {
 public:
  explicit test() {
    std::cout << "hello";
  }
  test(const test &t) = delete;
  test &operator=(const test &t) = delete;
};

template <std::string N>
class test2 {
 public:
  explicit test2() {
    std::cout << "hello" << N;
  }
  test2(const test2 &t) = delete;
  test2 &operator=(const test2 &t) = delete;
};

int main() {
  test<int>();
  test<int> a();//
  test2<"123">();
  test2<"456"> b();//
  test2<"789"> c;
  std::cout << Demangle(typeid(b).name());
  std::cout << TypeName(c);
}
