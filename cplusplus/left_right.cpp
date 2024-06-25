#include <memory>
#include <queue>
#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <iostream>
#include <atomic>
#include <thread>

void test(int &&r) {
  std::cout << "what";
}

int main() {
  int a = 1;
  int &&b = 3;
  test(a);
  test(b);
  test(std::move(b));
  return 0;
}
