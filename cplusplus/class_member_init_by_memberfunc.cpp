/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <utility>
#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>
#include <map>

class Test {
 public:
  int test() {
    return ++a;
  }
  void wave() {
    std::cout << a << b << c << d;
  }
 private:
  int a = 0;
  int b = test();
  int c = test();
  int d = test();
};

int main() {
  Test a;
  a.wave();
  return 0;
}
