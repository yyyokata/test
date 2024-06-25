/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <string>
#include <iostream>
#define RAW_IMPL(...) #__VA_ARGS__
#define RAW(str) RAW_IMPL(str)

int main() {
  std::cout << RAW("test":"123");
  return 0;
}
