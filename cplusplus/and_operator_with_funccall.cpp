/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <iostream>
bool test(int *num) {
  auto ret = (*num > 4);
  (*num)--;
  if (*num > 0) {
    ret = ret && test(num);
   // ret = test(num) && ret;
  }
  return ret;
}

int main() {
  int a = 6;
  test(&a);
  std::cout << a;
  return 1;
}
