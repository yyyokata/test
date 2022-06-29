/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <vector>
#include <algorithm>
#include <iostream>

int main() {
  std::vector<int> test {0,1,2,3,4,5,6,7,8};
  test.clear();
  test.push_back(9);
  std::cout << test.size() << *(test.end() -1) << *(test.begin());
  std::sort(test.begin(), test.end());
  std::cout << test.size();
  std::cout <<test.front();
  std::cout <<test.back();
  return 0;
}
