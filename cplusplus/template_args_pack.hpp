/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <vector>
#include <iostream>
template<class T, class...R>
std::vector<T> CollectArgs(const T &a, R... args);

template<class T, class...R>
std::vector<T> CollectArgs(const std::vector<T> &a, R... args);

template<class T>
std::vector<T> CollectArgs(const T &a) {
  return {a};
}

template<class T>
std::vector<T> CollectArgs(const std::vector<T> &a) {
  return a;
}

template<class T, class...R>
std::vector<T> CollectArgs(const T &a, R... args) {
  auto ret = CollectArgs(args...);
  ret.insert(ret.begin(), a);
  return ret;
}

template<class T, class...R>
std::vector<T> CollectArgs(const std::vector<T> &a, R... args) {
  auto ret = CollectArgs(args...);
  ret.insert(ret.begin(), a.begin(), a.end());
  return ret;
}

template<class T, class...R>
std::vector<int> Test(R... args);

template<>
std::vector<int> Test<bool>(int a, float b) {
  return {1};
}
