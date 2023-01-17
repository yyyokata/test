/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <iterator>
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
template<class Iter>
Iter p(Iter begin, Iter end) {
  for (auto ptr = begin; ptr < end; ++ptr) {
    if (*begin > *ptr) {
      std::iter_swap(begin, ptr);
      ++begin;
    }
  }
  return begin;
}

template<class Iter>
void sort_self(Iter begin, Iter end) {
  if (begin < end) {
    auto par = p(begin, end);
    sort_self(begin, par);
    sort_self(++par, end);
  }
}

int main() {
  std::vector<int> a = {5, 4, 3, 2, 1};
  std::array<float, 5> b = {3.2, 4.5, 6.7, 0.0, -2.f};
  sort_self(a.begin(), a.end());
  sort_self(b.begin(), b.end());
  for (auto e : a) {
    std::cout << e;
  }
  for (auto e : b) {
    std::cout << e;
  }
}
