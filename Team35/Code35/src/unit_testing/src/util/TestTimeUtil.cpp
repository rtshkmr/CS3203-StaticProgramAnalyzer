#include <util/TimeUtil.h>
#include <iostream>
#include "catch.hpp"

TEST_CASE("Testing time util") {
  std::string date_time = TimeUtil::GetDateTimeStr();
  std::string file_name_date_time = TimeUtil::GetDateTimeFileNameStr();
  std::cout << ("date time is: " + date_time + "\n");
  std::cout << ("file name version of date time is:" + file_name_date_time + "\n");
  int x = 1;
}