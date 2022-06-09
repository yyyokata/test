/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

// The function is only for generation test data
int main() {
  std::string buffer("thisisatestfile!fortestreadandwrite\0");

  std::ofstream file("input1", std::ios::binary);
  if (!file) {
    std::cout << "Failed to create file test.data" << std::endl;
    return -1;
  }
  file.write(buffer.data(), buffer.size());

  return 0;
}

