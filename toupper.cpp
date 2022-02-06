/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <string>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

int main() {
  std::string test {"0"};
  std::transform(test.begin(), test.end(), test.begin(), ::toupper);
  for (auto e_ : test) {
    std::cout << e_;
  }
  return 0;
}
