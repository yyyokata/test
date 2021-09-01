/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:Mon 12 Apr 2021 07:18:53 PM CST
 * File Name:./test7.hpp
 * Description: 
*************************************************************************/
#include <type_traits>
#include <iostream>

template<class T>
struct Op {
  Op(T t) {
    t_ = t;
  }
  T Operator() {
    return t_;
  }
  T *Operator() {
    return &t_;
  }
  T t_;
}

template<class T>
struct Test {
  Test(T t) {
    t_ = t;
  }
  T Operator() {
    return t_;
  }
  T t_;
}
