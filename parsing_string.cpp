/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:2021年11月22日 星期一 11时04分10秒
 * File Name:test12.cpp
 * Description: 
*************************************************************************/
#include <regex>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <ctype.h>
namespace std {

template<class T1, class T2>
std::ostream & operator << (std::ostream &os, const std::pair<T1, T2> &vec) {
  os << "[";
  os << vec.first << ", " << vec.second;
  os << "]";
  return os;
}

}  // namespace std
std::pair<std::string, int> ParseString(const std::string &name) {
  std::string n = name;
  int index = -1;
  for (int i = name.size() - 1; i >= 0; --i) {
    if (name[i] == '-') {
      bool has_index = true;
      auto tail =  name.substr(i+1);
      for (auto e_ : tail) {
        if (!isdigit(e_)) {
          // a walkaround for no exception
          has_index = false;
          break;
        }
      }
      if (has_index) {
        index = std::stoi(tail);
        n = name.substr(0, i);
      }
      break;
    }
  }
  return std::make_pair(n, index);
}



int main() {
  std::cout << ParseString("test");
  std::cout << ParseString("test-1024");
  std::cout << ParseString("test-23.4");
  std::cout << ParseString("test-asdf");
  std::cout << ParseString("test-");
}
