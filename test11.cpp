#include <iostream>
#include <string>
#include <vector>

class ITest {
 public:
  void wave() {
    std::cout << "test" << std::endl;
  }
};

class ITest1 : public ITest {
 public:
  ITest1(int arg1) {
    std::cout << "test" << std::endl;
    std::cout << arg1 << std::endl;
  }
};

class ITest2 : public ITest {
 public:
  ITest2(int arg1, double arg2) {
    std::cout << "test" << std::endl;
    std::cout << arg1 << " " << arg2 << std::endl;
  }
};

class TestCollection {
 public:
  template<class T, class = typename std::is_base_of<ITest, T>::type, typename... Args>
  T *AddIExtendNode(int arg1, const Args&... data) {
    T *ret = nullptr;
    ret = new T(arg1, data...);
    AddNode(ret);
    return ret;
  }
 protected:
  void AddNode(ITest *node) {
    tests.push_back(node);
  }
  std::vector<ITest *> tests;
};

int main() {
  TestCollection a;
  a.AddIExtendNode<ITest2>(int(1), double(3));
  a.AddIExtendNode<ITest1>(int(1));
  return 0;
}
