/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include "type_deduce.hpp"
#include "demangle.hpp"

int main() {
  auto t = Test<int, std::string>();
  t->wave();
  std::cout << Demangle(TypeName(t).c_str());
  return 0;
}

