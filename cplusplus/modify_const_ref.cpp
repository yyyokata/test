/*************************************************************************
 * Author: Nan Lin
 * Created Time:2021年01月28日 星期四 10时29分23秒
 * File Name:test2.cpp
 * Description: 
*************************************************************************/
#include <memory>
#include <queue>
#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <iostream>
void test(const int& ha) {
  ha = 3;
}

int main() {
  test(4);
  return 0;
}
