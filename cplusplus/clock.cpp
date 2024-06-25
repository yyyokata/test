/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <time.h>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;
int main() {
  typedef std::chrono::time_point<std::chrono::system_clock, chrono::nanoseconds> nanoclock;
  nanoclock t = std::chrono::system_clock::now();
  auto ct = system_clock::to_time_t(t);
  char tb [30];
  strftime(tb, 30, "%Y-%m-%d %H:%M:%S", localtime(&ct));
  std::cout << tb;
  return 0;
}
