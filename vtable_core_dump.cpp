#include <iostream>
class Base {
public:
  void (*Test)(Base *impl);
};

template<class UserImpl>
class FallBack : public Base {
public:
  FallBack() : Base{Test} {}
  static inline void Test(Base *impl);
};

template<class UserImpl>
void FallBack<UserImpl>::Test(Base *impl) {
  static_cast<UserImpl *>(impl)->Test();
}

template<class UserImpl, class Concrete>
class External : public FallBack<UserImpl> {
public:
};

class MyImpl : public External<MyImpl, int> {
public:
  void Test() {
    std::cout << "?";
  }
  virtual void Test2() {
    std::cout << "?";
  }
};

void Use() {
  void *element = new(malloc(sizeof(MyImpl))) MyImpl();
  std::cout << element << " " << sizeof(MyImpl) << " " << sizeof(Base) << std::endl;
  MyImpl *element2 = static_cast<MyImpl *>(element);
  ((Base *)element2)->Test((Base *)element2);
}

int main() {
  Use();
}
