#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <utility>
#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>
#include <map>

#define UNPACK(...) __VA_ARGS__
#define TEST(name, type, ...) \
  UNPACK type __##name {__VA_ARGS__}

int main() {
  std::list<int> a = {1,2,3,4,5,6};
  std::vector<int> b = {a.begin(), a.end()};
  auto c = std::vector<int64_t>(a.begin(), a.end());
  for (auto e_ : b) {
    std::cout << e_ << "\n";
  }
  for (auto e_ : c) {
    std::cout << e_ << "\n";
  }
  std::map<std::string, double> test {std::make_pair("test", 3.4)};
  TEST(test, (std::map<std::string, double>), std::make_pair("test5", 5.6));
  TEST(test2, (std::vector<int>), b);
  std::cout << (0 && !1);
  bool t = 0;
  bool l = 1;
  std::cout << (t && !l);
  std::cout << (0 && 0);
}
