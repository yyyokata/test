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
  void wave() {
    std::cout << "test1" << "\n";
  }
  virtual void wave2() {
    std::cout << "test2" << "\n";
  }

  virtual void wave3() = 0;
};

class Test1 : public Test {
 public:
  void wave() {
    std::cout << "subtest1" << "\n";
  }
  void wave2() {
    std::cout << "subtest2" << "\n";
  }
  void wave3() {
    std::cout << "subtest2" << "\n";
  }
};

class Test2 : public Test {
 public:
  void wave2() override {
    std::cout << "sub2test2" << "\n";
  }
  void wave3() override {
    std::cout << "sub2test2" << "\n";
  }
};

int main() {
  Test1 *test1 = new Test1();
  Test2 *test2 = new Test2();
  test1->wave();
  test1->wave2();
  test1->wave3();
  test2->wave();
  test2->wave2();
  test2->wave3();
  Test* t1 = test1;
  Test* t2 = test2;
  t1->wave();
  t1->wave2();
  t1->wave3();
  t2->wave();
  t2->wave2();
  t2->wave3();
  delete test1;
  delete test2;
  //subtest1
  //subtest2
  //subtest2
  //test1
  //sub2test2
  //sub2test2
  //test1
  //subtest2
  //subtest2
  //test1
  //sub2test2
  //sub2test2
  //From cppreference (en.cppreference.com/w/cpp/language/virtual): "If some member function vf is declared as virtual in a class Base, and some class Derived, which is derived, directly or indirectly, from Base, has a declaration for member function with the same name parameter type list (but not the return type) cv-qualifiers ref-qualifiers Then this function in the class Derived is also virtual (whether or not the keyword virtual is used in its declaration) and overrides Base::vf (whether or not the word override is used in its declaration)."
}
