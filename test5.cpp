/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:Fri 26 Mar 2021 11:47:11 AM CST
 * File Name:test5.cpp
 * Description: 
*************************************************************************/
#include <type_traits>
#include <iostream>

template<class A>
class B {
 public:
  A test() {
    A a{};
    return a;
  }
};

class C : public B<int> {
};

class D{};

template<class T, class = typename std::is_base_of<B<int>, T>::type>
void test(T a) {
  std::cout << a.test();
}

int main() {
  C c;
  test(c);
  return 0;
}
