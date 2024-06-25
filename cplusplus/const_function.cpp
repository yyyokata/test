class C {
 public:
  int get() const {
    return 1;
  }
};

class B {
 public:
  B() : c_(new C()) {}
  C* getC() {
    return c_;
  }
 private:
  C *c_;
};

class A {
 public:
  A() : b_(new B()) {}
  int get() const {
    return b_->getC()->get();
  }
 private:
  B* b_;
};

int main() {
  A a;
  a.get();
}
