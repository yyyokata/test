#include <string>
#include <iostream>
int main() {
  bool test = false;
  std::string out = std::string("a") + (test ? "b" : "v");
  std::cout << out;
  return 0;
}
