/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <vector>
#include <iostream>

template<class...T>
void testcore(const T &...t);

template<>
void testcore(const int &a, std::vector<int>* const& t) {
  t->push_back(a);
}

template<class...T>
class test {
 public:
  virtual void testfunc(const T &...t) = 0;
};

template <>
class test<float, std::vector<int>*> : public test<int, std::vector<int>*> {};

template<class...T>
class test3 : public test<T...> {
 public:
  void testfunc(const T &...t) override {
    return testcore(t...);
  }
};


int main() {
  std::vector<int> ids;
  testcore(1, &ids);
  std::cout << ids[0];
  test3<int, std::vector<int>*> t;
  t.testfunc(1, &ids);
  std::cout << ids[1];
}
