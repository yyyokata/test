/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <iostream>
#include <memory>

int main () {
  if (!(auto a = std::unique_ptr<int>(nullptr))) {
    std::cout << "?";
  } else {
    std::cout << "1";
  }
  if (auto b = std::unique_ptr<int>(new int(12))) {
    std::cout << "?";
  } else {
    std::cout << "1";
  }
  return 0;
}
