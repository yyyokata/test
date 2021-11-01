/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:2021年09月15日 星期三 17时31分40秒
 * File Name:./test9.cpp
 * Description: 
*************************************************************************/
#include <vector>
#include <string>
#include <iostream>

enum class test_class : int {
  test = 1,
};

template<typename T>
struct TEST {
};

bool test2(int *t) {
  *t = 1;
  return true;
}

int main() {
  std::string a = "abcdefg";
  std::vector<char> b = {a.begin(), a.end()};
  for (auto e_ : b) {
    std::cout << e_ << "\n";
  }
  std::vector<int> d = {0,2};
  test2(&d[0]);
  for (auto e_ : d) {
    std::cout << e_ << "\n";
    ++e_;
    std::cout << e_ << "\n";
  }
}
