/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <type_traits>
#include <iostream>
#include <vector>

template <typename T>
struct test {
};

template <typename T>
struct test <std::vector<T>> {
  void wave() {
    std::cout << "2";
  }
};

template <typename T>
struct test <std::vector<std::vector<std::vector<T>>>> {
};

int main() {
  test <std::vector<std::vector<int>>> a;
  a.wave();
  return 0;
}
