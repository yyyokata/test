/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <string>
#include <iostream>
#include <vector>
#include <array>
#define RegTypesOnce(...) \
  static std::vector<std::string> test { #__VA_ARGS__ }; \


int main () {
  RegTypesOnce(test4, test2, test3);
  for (auto e_ : test) {
    std::cout << e_ << std::endl;
  }
}
