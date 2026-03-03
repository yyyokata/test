#include <vector>
#include <iostream>

int main() {
  std::vector<int> idxs {1, 2, 3};
  for (auto idx : idxs) {
    switch (idx) {
      case 2:
        break;
      default:
        std::cout << idx;
    }
  }
}
