#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <utility>
#include <unordered_map>
#include <string>
#include <functional>
#include <iostream>

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
}
