/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <functional>
#include <type_traits>
#include <vector>
#include <string>
#include <utility>
#include <map>

template<typename T>
using BaseType = std::remove_cv<typename std::remove_pointer<typename std::remove_reference<T>::type>::type>;

template <typename T1, typename T2>
using Same = std::is_same<typename std::decay<T1>::type, typename std::decay<T2>::type>;

template <typename T1, typename T2>
using IsSame = std::enable_if<std::is_same<typename std::decay<T1>::type, typename std::decay<T2>::type>::value>;

template <typename T1, typename T2>
using IsNSame = std::enable_if<!std::is_same<typename std::decay<T1>::type, typename std::decay<T2>::type>::value>;

template <size_t index, typename Arg, typename... Args>
struct ParamPackType;

template <size_t index, typename Arg, typename... Args>
struct ParamPackType : ParamPackType<index - 1, Args...> {};

template <typename Arg, typename... Args>
struct ParamPackType<0, Arg, Args...> {
  typedef Arg type;
};

template <typename T>
struct function_traits;

template <typename RetType, typename... Args>
struct function_traits<RetType(Args...)> {
  typedef RetType return_type;
  typedef std::function<RetType(Args...)> func_type;
  enum { size = sizeof...(Args) };

  template <size_t index>
  struct Param {
    typedef typename ParamPackType<index, Args...>::type type;
    typedef typename BaseType<typename ParamPackType<index, Args...>::type>::type base_type;
  };
};

template <typename ClassType, typename RetType, typename... Args>
struct function_traits<RetType (ClassType::*)(Args...) const> : function_traits<RetType(Args...)> {};

template <typename LambdaFunc>
struct function_traits : function_traits<decltype(&LambdaFunc::operator())> {};

template <typename RetType, typename... Args>
struct function_traits<RetType(Args...) const> : function_traits<RetType(Args...)> {};

template <typename RetType, typename... Args>
struct function_traits<RetType(*)(Args...)> : function_traits<RetType(Args...)> {};

template <typename T>
using Checker = std::function<bool(const T&)>;
// some wrapper for common types, we can use them recursively with typedef/using
// F->T uses some traits to exact first base type of checker function
// In gcc > 6.0, template can be reduced automatically, but we need some walkaround
// for gcc 5.4, so here it is.
template <class F, class T = typename function_traits<F>::template Param<0>::base_type>
Checker<std::vector<T>> vector_checker(F checker) {
  Checker<std::vector<T>> lambda = [checker](const std::vector<T> &value) -> bool {
    for (auto &e_ : value) {
      if (!checker(e_)) {
        return false;
      }
    }
    return true;
  };
  return lambda;
}

template <class F1, class F2, class T1 = typename function_traits<F1>::template Param<0>::base_type, class T2 = typename function_traits<F2>::template Param<0>::base_type>
Checker<std::map<T1, T2>> map_checker(F1 key_checker, F2 value_checker) {
  Checker<std::map<T1, T2>> lambda = [key_checker, value_checker](const std::map<T1, T2> &value) -> bool {
    for (auto &e_ : value) {
      if (!key_checker(e_.first) || !value_checker(e_.second)) {
        return false;
      }
    }
    return true;
  };
  return lambda;
}

