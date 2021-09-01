/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:Mon 12 Apr 2021 06:46:20 PM CST
 * File Name:./test7.cpp
 * Description: 
*************************************************************************/
#include "./test7.hpp"
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
