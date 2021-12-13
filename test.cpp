/*************************************************************************
 * Author: Nan Lin
 * Created Time:2021年01月20日 星期三 21时22分38秒
 * File Name:test.cpp
 * Description: 
*************************************************************************/
#include <memory>
#include <queue>
#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <iostream>

template <size_t ...I>
struct index_sequence {};

template <size_t N, size_t ...I>
struct make_index_sequence : public make_index_sequence<N-1, N-1

template<typename F>
int test_func(F&& func, std::vector<int> args) {
  for (auto &e_ : args) {
    func = std::bind(func, e_);
  }
  return func();
}

int test2(int a, int b, int c, float d = 1.0) {
  std::cout << d << std::endl;
  return a+b+c;
}

int main() {
  std::cout << test_func(test2, {3,4,5});
  return 0;
}
