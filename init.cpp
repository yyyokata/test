/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <string>
#include <iostream>
#include <array>

using Node = INode;

int main() {
  int a = {(std::cout << "?", 1, std::cout << "1", 0)};
  int b = (std::cout << "ha", a = a + 1);
  (std::cout << a, ++a, std::cout << a);
  return a;
}
