/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <iostream>
//class Derived;
//
//class Base {
//  public:
//  Derived d();
//  protected:
//  void test() {
//    idx++;
//  }
//  friend Derived::Derived(Base *);
//  int idx = 0;
//};
//
//class Derived {
//  public:
//  Derived(Base *b) : base_(b) {}
//  void derived_test() {
//    b.test();
//  }
//  friend void Base::test();
//  private:
//  Base *base_;
//};
//
//Derived Base::d() {
//  return Derived(this);
//}

class Base {
  public:
  void test() {
    idx++;
  }
  void test(const Base &another) {
    std::cout << another.idx;
  }
  private:
  int idx = 0;
};


int main() {
  Base a, b;
  a.test();
  b.test(a);
}
