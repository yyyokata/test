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

struct test2 {
  test2() {
    std::cout << "hello2";
  }
};

int main() {
  test<int>();
  test<int> a();//
  test2();
  test2 b();//
  test2 c;
  std::cout << Demangle(typeid(b).name());
  std::cout << TypeName(c);
}
