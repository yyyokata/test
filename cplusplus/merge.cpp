/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/

template<class T>
T *Merge(T* t) {
  return t;
}

template<class T, class... Args>
T *Merge(T* t, Args...ts) {
  if (auto p = Merge(ts)) {
    return p;
  }
  return t;
}

template<class Op>
Op *Test(Op *in) {
  return in;
}

void *ForEachTest(void *
