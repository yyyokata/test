/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:2021年11月26日 星期五 15时01分30秒
 * File Name:test13.cpp
 * Description: 
*************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#define WAVE wave

class ITest {
 public:
  virtual void wave() = 0;
};

class ITest1 : public ITest {
 public:
  void wave() override {
    std::cout << "test1" << std::endl;
  }
};

class ITest2 : public ITest1 {
 private:
  void wave() override {
    std::cout << "test2" << std::endl;
  }
};

int main() {
  ITest2 t;
  ITest1 *t1 = &t;
  ITest *t2 = t1;
  ITest *t3 = &t;
  t1->WAVE();
  t2->WAVE();
  t3->WAVE();
  return 0;
}
