/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Author: Nan Lin
 * Created Time:2021年08月19日 星期四 20时40分42秒
 * File Name:test8.cpp
 * Description: 
*************************************************************************/
#include <cmath>
#include <memory>
#include <algorithm>
#include <iostream>

double pow(int base, int pos) {
  return std::pow(double(base), double(pos));
}

double to_s(int pos, double scale) {
  return pow(2, pos)/scale;
}

std::pair<double, double> to_range1(double scale, int z) {
  double max = pow(2, 7);
  double min = -pow(2, 7) + 1;
  double dmax = std::abs(scale * min);
  double dmin = -dmax;
  return std::make_pair(dmin, dmax);
}

std::pair<double, double> to_range2(double scale, int z) {
  double max = std::numeric_limits<int8_t>::max(); // 128
  double min = std::numeric_limits<int8_t>::min(); // -127
  double dmin = - scale * (double)z - max / (max - min);
  double dmax = scale * (max - min) + dmin;
  return std::make_pair(dmin, dmax);
}

double to_scale(double dmin, double dmax) {
  double abs_max = std::abs(dmax) > std::abs(dmin) ? std::abs(dmax) : std::abs(dmin);
  double scale = 1.0f;
  int pos = 0;
  int scale_var = pow(2, 7) - 1;
  pos = static_cast<int>(std::floor(std::log2(abs_max)) - 6);
  scale = static_cast<double>(pow(2, pos) * scale_var / abs_max);
  return to_s(pos,scale);
}

std::pair<double, int> to_scale2(double dmin, double dmax) {
  dmin = dmin > 0 ? 0 : dmin;
  dmax = dmax < 0 ? 0 : dmax;
  double offset_ori = (-pow(2, 7) - dmin * (pow(2, 8) - 1)) / (dmax - dmin);
  std::cout << "ori offset " << offset_ori << std::endl;
  int offset = std::round(offset_ori);
  double new_s = (dmax - dmin) / (pow(2, 8) - 1);
  std::cout << "new scale " << new_s << std::endl;
  int pos = std::floor(std::log2(dmax - dmin)) - 7;
  double scale = (pow(2, pos + 8) - pow(2, pos)) / (dmax - dmin);
  std::cout << "ori pos " << pos << " and scale " << scale << std::endl;
  return std::make_pair(to_s(pos,scale), offset);
}

int main() {
  auto s = to_s(-6, 1.35763);
  int z = 40;
  std::cout << s << " " << z << std::endl;
  auto mm = to_range1(s, z);
  std::cout << mm.first << " " << mm.second << std::endl;
  double as = to_scale(mm.first, mm.second);
  std::cout << as << std::endl;
//  auto s = to_s(-6, 1.35763);
//  int z = 40;
//  std::cout << s << " " << z << std::endl;
//  auto mm = to_range2(s, z);
//  std::cout << mm.first << " " << mm.second << std::endl;
//  std::pair<double, int> as = to_scale2(mm.first, mm.second);
//  std::cout << as.first << " " << as.second << std::endl;
//  std::pair<double, int> as2 = to_scale2(-0.9623, 3.897095);
//  std::cout << as2.first << " " << as2.second << std::endl;
//  auto s = to_s(-6, 1.35763);
//  int z = 40;
//  std::cout << s << " " << z << std::endl;
//  auto mm = to_range(s, z);
//  std::cout << mm.first << " " << mm.second << std::endl;
//  std::pair<double, int> as = to_scale2(mm.first, mm.second);
//  std::cout << as.first << " " << as.second << std::endl;
  return 0;
}
