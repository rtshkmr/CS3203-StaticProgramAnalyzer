#include <sstream>
#include "TimeUtil.h"
#include <time.h>
#include <chrono>
#include <ctime>
#include <cassert>

/**
 * Returns a formatted version of the current date time
 * @param tm  time struct
 * @return  string format of the current day, date, month, year,seconds
 */
std::string TimeUtil::FormatDateTime(const tm* tm) {
//  auto now = std::chrono::system_clock::now();
//  std::time_t current_time = std::chrono::system_clock::to_time_t(now);
//  std::ctime(&current_time);



  static std::string date_delim = "/";
  static std::string time_delim = ":";
  std::stringstream string_stream;

  // day:
  int day = (* tm).tm_wday;
  switch (day) {
    case 0: string_stream << "Sun ";
      break;
    case 1: string_stream << "Mon ";
      break;
    case 2: string_stream << "Tue ";
      break;
    case 3: string_stream << "Wed ";
      break;
    case 4: string_stream << "Thu ";
      break;
    case 5: string_stream << "Fri ";
      break;
    case 6: string_stream << "Sat ";
      break;
    default:assert(false);
      break;
  }
  // add date:
  int date = (* tm).tm_mday;

  if (date < 10) {
    string_stream << "0" << date << date_delim;
  } else {
    string_stream << date << date_delim;
  }
  // add month:
  int month = (* tm).tm_mon + 1;
  if (month < 10) {
    string_stream << "0" << month << date_delim;
  } else {
    string_stream << month << date_delim;
  }
  int year = (* tm).tm_year + 1900;
  // add year:
  string_stream << year << " ";
  // add timestamp:
  int hour = (* tm).tm_hour;
  string_stream << hour << time_delim;
  int min = (* tm).tm_min;
  if (min < 10) {
    string_stream << "0" << min << time_delim;
  } else {
    string_stream << min << time_delim;
  }
  // add seconds :
  int seconds = (* tm).tm_sec;
  if (seconds < 10) {
    string_stream << "0" << seconds;
  } else {
    string_stream << seconds;
  }
  return string_stream.str();
}

/**
 * Formats the date time into a string format that is file-name-safe.
 * @param tm
 * @return
 */
std::string TimeUtil::FormatDateTimeFileName(const tm* tm) {

  std::stringstream string_stream;
  static std::string file_name_field_delim = "_";
  int year = (* tm).tm_year + 1900;
  int month = (* tm).tm_mon + 1;
  int day = (* tm).tm_mday;
  int hour = (* tm).tm_hour;
  int sec = (* tm).tm_sec;

  // year
  string_stream << year << file_name_field_delim;
  if (month < 10) {
    string_stream << "0" << month << file_name_field_delim;
  } else {
    string_stream << month << file_name_field_delim;
  }
  if (day < 10) {
    string_stream << "0" << day << file_name_field_delim;
  } else {
    string_stream << day << file_name_field_delim;
  }

  // time
  string_stream << hour << file_name_field_delim;
  if (hour < 10) {
    string_stream << "0" << hour << file_name_field_delim;
  } else {
    string_stream << hour << file_name_field_delim;
  }
  if (sec < 10) {
    string_stream << "0" << sec;
  } else {
    string_stream << sec;
  }
  return string_stream.str();
}

std::string TimeUtil::GetDateTimeStr() {
  time_t t = time(nullptr);
  struct tm* timeinfo;
  time(& t);
  timeinfo = localtime(& t);
  return FormatDateTime(timeinfo);
}
std::string TimeUtil::GetDateTimeFileNameStr() {
  time_t t = time(nullptr);
  struct tm* timeinfo;
  time(& t);
  timeinfo = localtime(& t);
  return FormatDateTimeFileName(timeinfo);
}
