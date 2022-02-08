/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#ifndef STRING_WRAP_HPP_
#define STRING_WRAP_HPP_
#include <string>
#include <cstring>
#include <cassert>
#include <vector>

class String {
 public:
  String(const std::string &in) : data_(in.begin(), in.end() + 1) {}
  String(const char *in) : data_(in, in + std::strlen(in) + 1) {}
  String(const char *in, size_t len) : String(std::string(in, len)) {}
  String() : String(""){};

  operator std::string() const { return std::string(begin(), end()); }
  bool operator==(const char* str) const { return std::string(*this) == std::string(str); }
  bool operator==(const std::string &str) const { return std::string(*this) == str; }
  bool operator==(const String& str) const { return data_ == str.data_; }

  bool operator!=(const char* str) const { return std::string(*this) != std::string(str); }
  bool operator!=(const std::string &str) const { return std::string(*this) != str; }
  bool operator!=(const String& str) const { return data_ != str.data_; }

  bool empty() const { return size() == 0; }
  size_t size() const { return data_.size() - 1; }

  char &at(size_t pos) {
    assert(pos < size());
    return data_.at(pos);
  }
  const char &at(size_t pos) const {
    assert(pos < size());
    return data_.at(pos);
  }
  char& operator[](size_t pos) { return data_[pos];}
  const char &operator[](size_t pos) const { return data_[pos]; }

  char &front() { return data_.front(); }
  const char &front() const { return data_.front(); }
  char &back() { return data_.at(size() - 1); }
  const char &back() const { return data_.at(size() - 1); }

  const char* c_str() const { return data_.data(); }

  std::vector<char>::iterator begin() { return data_.begin(); }
  std::vector<char>::const_iterator begin() const { return data_.begin(); }
  std::vector<char>::const_iterator cbegin() const { return data_.cbegin(); }

  // remove the last \0
  std::vector<char>::iterator end() { return data_.end() - 1; }
  std::vector<char>::const_iterator end() const { return data_.end() - 1; }
  std::vector<char>::const_iterator cend() const { return data_.cend() - 1; }

  int compare(const std::string &str) const { return std::string(*this).compare(str); }

 private:
  std::vector<char> data_;
};


#endif  // STRING_WRAP_HPP_
