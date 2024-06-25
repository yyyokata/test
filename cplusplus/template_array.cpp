/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <vector>
#include <iostream>

template <typename T, size_t SIZE>
void print(const T(&array)[SIZE], const T(&array2)[SIZE]) {
  for (size_t i = 0; i < SIZE; i++) {
    std::cout << array[i] << " ";
    std::cout << array2[i] << " ";
  }
}

int main() {
  int x[] = {1, 2, 3};
  std::vector<int> y{1, 2, 3};
  print(x, y.data());
}
