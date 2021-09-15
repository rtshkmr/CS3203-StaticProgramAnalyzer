#ifndef AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_UTIL_H_
#define AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_UTIL_H_

#pragma once

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

//todo: rename tostring to follow codestyle rules
/**
 * Tempate toString function
 * @tparam T
 * @param t
 * @return
 */
template<typename T>
inline std::string tostring(T t) {
  std::stringstream string_stream;
  string_stream << t;
  return string_stream.str();
}

#endif //AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_UTIL_H_
