#ifndef AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_UTIL_H_
#define AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_UTIL_H_

#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

template<typename T>
//todo: rename tostring to follow codestyle rules
inline std::string tostring(T t) {
  std::stringstream basic_stringstream;
  basic_stringstream << t;
  return basic_stringstream.str();
}

#endif //AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_UTIL_H_
