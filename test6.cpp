/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:2021年03月29日 星期一 21时22分47秒
 * File Name:test6.cpp
 * Description: 
*************************************************************************/
#include <type_traits>
#include <iostream>

template<class T, class P>
class test;

template<class T>
class test<T, typename std::is_pointer<T>::type>
{
 public:
  test() {
    std::cout << "pointer";
  }
};

template<class T>
class test<T, typename std::enable_if<std::is_pointer<T>::value>::type>
{
 public:
  test() {
    std::cout << "not pointer";
  }
};

template<class T>
class test2 : 

int main() {

}
