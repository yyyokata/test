/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include "./template_args_pack.hpp"

int main() {
  auto a = CollectArgs(1, 2, std::vector<int>({3,4,5}), 6, std::vector<int>({7, 8}));
  for (auto i : a) {
    std::cout << i;
  }
  Test<bool>(1, 3.f);
}
