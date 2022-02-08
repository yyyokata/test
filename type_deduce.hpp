/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include "./template_type_deduce.hpp"
#include "./string_wrap.hpp"
#include <iostream>

template<typename T>
using replace = std::conditional_t<std::is_same<std::string, T>::value, String, T>;

template<typename... T>
using type = std::tuple<replace<T>...>;

template <typename... Args>
class TT {
 public:
  void wave() {
    std::cout << "not here";
  }
};

template <>
class TT<int, std::string> {
 public:
  void wave() {
    std::cout << "not here either";
  }
};

template <>
class TT<int, String> {
 public:
  void wave() {
    std::cout << "here";
  }
};

template <typename... Args>
TT<replace<Args>...> *Test() {
  return new TT<replace<Args>...>();
}
