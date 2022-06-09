/*************************************************************************
 * Copyright (C) [2020-2023] by Cambricon, Inc.
 * Description: 
 *************************************************************************/
#include "./string_wrap.hpp"
#include "functional"

using PluginNameConverter = std::function<std::string(const std::string &)>;

bool LoadRegistedPlugin(PluginNameConverter converter) {
  std::function<String(const String &)> wrap = [](const String &ha) {
    return converter(ha);
  }
}

