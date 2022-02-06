/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:2021年11月28日 星期日 22时59分08秒
 * File Name:./test14.cpp
 * Description: 
*************************************************************************/
#include <iostream>
#include <string>
#include <vector>
class ITest {
 public:
  virtual void wave() {
    std::cout << "test" << std::endl;
  }
};

class ITest1 : public ITest {
 public:
  void wave() override {
    std::cout << "test1" << std::endl;
  }
};

class ITest2 : public ITest {
 public:
  void wave() override {
    std::cout << "test2" << std::endl;
  }
};

template<class T, class = typename std::is_base_of<ITest, T>::type>
void wave_out(T t) {
  return t.wave();
}

template<class T, class = typename std::is_base_of<ITest, T>::type, class... R>
void wave_out(T t, R... rest) {
  t.wave();
  return wave_out(rest...);
}

int main() {
  ITest t;
  ITest1 t1;
  ITest2 t2;
  wave_out(t, t1, t2);
  return 0;
}

