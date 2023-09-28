/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#ifndef __PRIVATE_WRAPPER_H_
#define __PRIVATE_WRAPPER_H_

class Base {
  public:
  Derived d();
  protected:
  void test();
  int idx = 0;
};

class Derived : private Base {
  public:
  void derived_test();
  private:
  Derived(Base *b) : base_(b) {}
  Base *base_;
};

#endif  // __PRIVATE_WRAPPER_H_
