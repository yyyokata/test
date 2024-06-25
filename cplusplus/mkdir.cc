/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
std::vector<std::string> StringSplit(const std::string &s, const std::string &delimiter) {
  std::vector<std::string> result;
  std::string::size_type pos1, pos2;
  pos2 = s.find(delimiter);
  pos1 = 0;
  while (std::string::npos != pos2) {
    result.push_back(s.substr(pos1, pos2 - pos1));
    pos1 = pos2 + delimiter.size();
    pos2 = s.find(delimiter, pos1);
  }
  if (pos1 != s.length()) {
    result.push_back(s.substr(pos1));
  }
  return result;
}

bool CreateFolder(const std::string &file_path) {
  std::string cur_dir = "";
  std::vector<std::string> dirname_vec = StringSplit(file_path, "/");
  for (auto &dirname : dirname_vec) {
    cur_dir = cur_dir + dirname + "/";
    if (access(cur_dir.c_str(), R_OK) != 0) {
      if (mkdir(cur_dir.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
        return false;
      }
    }
  }
  return true;
}

int main() {
  std::cout << CreateFolder("/test");
}
