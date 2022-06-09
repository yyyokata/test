/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <string>
#include <iostream>
#include <array>
template<class T>
struct Golden {
  std::array<T, 2> unchangable {{T(1), T(0)}};
};

int main() {
  Golden<int> test {};
  return 0;
}
