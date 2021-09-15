#ifndef AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_TIMEUTIL_H_
#define AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_TIMEUTIL_H_

#pragma once
#include <string>
class TimeUtil {
 public:
  static std::string FormatDateTime(const tm& tm);
  static std::string FormatDateTimeFileName(const tm& time);
  static std::string GetDateTimeStr();
  static std::string GetDateTimeFileNameStr();
};

#endif //AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_TIMEUTIL_H_
