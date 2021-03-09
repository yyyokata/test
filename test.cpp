/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *************************************************************************/
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
