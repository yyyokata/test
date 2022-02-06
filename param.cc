#include "param.h"

std::string binary_name = "UserBinary";

Args ArrangeArgs(const int argc, char *argv[], int offset) {
  if (argc == 0) {
    SLOG(ERROR) << "Empty command line";
    abort();
  }
  ArgValue all_args {};
  all_args.assign(argv, argv + argc);
  binary_name = all_args[0];
  Args ret {};
  // Rearrangment
  std::string head = "";
  for (auto iter = all_args.begin() + offset; iter != all_args.end(); ++iter) {
    if (iter->find("--") == 0) {
      head = iter->substr(2);
      if (ret.find(head) != ret.end()) {
        SLOG(ERROR) << "Duplicated command line arg: " << head
                    << " for " << binary_name;
        abort();
      }
    } else if (!head.empty()) {
      ret[head].push_back(*iter);
    } else {
      SLOG(ERROR) << "Unknown command line arg: " << *iter
                  << " for " << binary_name;
      abort();
    }
  }
  return ret;
}

std::string Trim(std::string s) {
  if (!s.empty()) {
    s.erase(0, s.find_first_not_of(" "));
  }
  if (!s.empty()) {
    s.erase(s.find_last_not_of(" ") + 1);
  }
  return s;
}

ArgBase::~ArgBase() {
  if (next_) {
    delete next_;
  }
}

void ArgBase::SetNext(ArgBase *next) {
  if (next_) {
    next_->SetNext(next);
  } else {
    next_ = next;
  }
}

bool ArgBase::ReadIn(Args *obj) {
  SLOG(ERROR) << name_ << "read";
  auto ret = UpdateArg(obj);
  if (next_) {
    ret = next_->ReadIn(obj) && ret;
  }
  return ret;
}

std::string ArgBase::DebugString() const {
  std::string s {};
  s += WriteOut();
  if (next_) {
    s += next_->DebugString();
  }
  return s;
}

std::string ArgBase::Synopsis() const {
  std::stringstream ss;
  ss << "Synopsis:\n";
  ss << "          " << binary_name << " ";
  ss << SynopCollect(0, false);
  ss << "\nOptions:\n";
  ss << SynopCollect(10, true);
  return ss.str();
}

std::string ArgBase::SynopCollect(int align, bool detailed) const {
  std::string s {};
  s += Doc(align, detailed);
  if (next_) {
    s += next_->SynopCollect(align, detailed);
  }
  return s;
}

std::ostream &operator<<(std::ostream &os, const ArgBase &arg) {
  return os << arg.DebugString();
}

void ArgListBase::ReadIn(Args *args) {
  auto res = front->ReadIn(args);
  if (args->size() > 0) {
    SLOG(ERROR) << "Unrecognized args:";
    for (auto e_ : (*args)) {
      SLOG(ERROR) << "--" << e_.first;
    }
    res = false;
  }
  if (!res) {
    SLOG(ERROR) << Synopsis();
    abort();
  }
}

std::string ArgListBase::DebugString() const{
  return front->DebugString();
}

std::string ArgListBase::Synopsis() const {
  return front->Synopsis();
}

ArgListBase::~ArgListBase() {
  if (front) {
    delete front;
    front = nullptr;
  }
}

std::ostream &operator<<(std::ostream &os, const ArgListBase &arg) {
  return os << arg.DebugString();
}
