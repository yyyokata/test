#include <iostream>

namespace mlir {
namespace utils {
int a = 0;
}
namespace test {
namespace utils {
int b = 1;
}
void test() {
  std::cout << utils::a;
}
}
}

