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

template<typename T, typename T2, bool i = std::is_integral<T2>::value && !std::is_integral<T>::value>
struct test2 {};

template<typename T, typename T2>
struct test2<T, T2, true> {
  void operator()() {
    std::cout << "!";
  }
};

template<typename T, typename T2>
struct test2<T, T2, false> {
  void operator()() {
    std::cout << "?";
  }
};

template<typename T, typename T2>
void dotest2() {
  test2<T, T2>()();
}

int main() {
  test <std::vector<std::vector<int>>> a;
 // a.wave();
  dotest2<float, int>();
  dotest2<int, int>();
  return 0;
}
