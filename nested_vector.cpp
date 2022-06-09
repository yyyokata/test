/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <vector>
#include <iostream>

int main() {
  std::vector<std::vector<int>> test(3, std::vector<int>(3,3));
  std::vector<int> test2(9,3);
  auto ptr = test.data();
  for (int i = 0; i < 9; ++i) {
    std::cout << *((int *)ptr + i);
  }
  return 0;
}
