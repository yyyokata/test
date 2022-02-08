/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#ifndef DEMANGLE_HPP_
#define DEMANGLE_HPP_
#include <memory>
#include <cxxabi.h>
#include <string>
#include <typeinfo>

/*
 * A simple interface to demangle typename.
 */
std::string Demangle(const char *name) {
  int status = 0;
  std::unique_ptr<char, void (*)(void*)> res{
    abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
  return (status == 0) ? res.get() : name;
}

template <typename T>
std::string TypeName() {
  return Demangle(typeid(T).name());
}

template <typename T>
std::string TypeName(const T &t) {
  return Demangle(typeid(t).name());
}
#endif  // DEMANGLE_HPP_
