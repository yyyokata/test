/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/

class Interface {
 public:
  Interface(InterfaceImpl *impl);
  int GetNum() {
    return impl->GetNum();
  }
 private:
  InterfaceImpl *impl;
};

class InterfaceImpl {
 public:
  int GetNum() {
    return num;
  }
 protected:
  int num = 0;
};

class DerivedInterface : public Interface {
 public:
  DerivedInterface(InterfaceImpl *impl) : Interface(impl);
  int GetNumPlus() {
    return impl->GetNum() + 1;
  }
}

DerivedInterface *NewNode(int num) {

}
