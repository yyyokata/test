#include <iostream>
#include <string>

template <typename F>
bool testF(F const &f) {
  bool test = false;
  return f(&test);
}

int main() {
  bool test2 = true;
  auto f = [&](bool *t) -> bool {
    std::cout << *t << "!!";
    std::cout << test2 << "!?";
    return t;
  };
  testF(f);
  return 0;
}
