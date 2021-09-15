#include <sstream>
#include "TimeUtil.h"
#include <time.h>

/**
 * Returns a formatted version of the current date time
 * @param tm  time struct
 * @return  string format of the current day, date, month, year,seconds
 */
std::string TimeUtil::FormatDateTime(const tm& tm) {
  static std::string date_delim = "/";
  static std::string time_delim = ":";
  std::stringstream string_stream;

  // day:
  switch (tm.tm_wday) {
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
  }
  // add date:
  if (tm.tm_mday < 10) {
    string_stream << "0" << tm.tm_mday << date_delim;
  } else {
    string_stream << tm.tm_mday << date_delim;
  }
  // add month:
  if (tm.tm_mon + 1 < 10) {
    string_stream << "0" << tm.tm_mon + 1 << date_delim;
  } else {
    string_stream << tm.tm_mon + 1 << date_delim;
  }
  // add year:
  string_stream << tm.tm_year + 1900 << " ";
  // add timestamp:
  string_stream << tm.tm_hour << time_delim;
  if (tm.tm_min < 10) {
    string_stream << "0" << tm.tm_min << time_delim;
  } else {
    string_stream << tm.tm_min << time_delim;
  }
  // add seconds :
  if (tm.tm_sec < 10) {
    string_stream << "0" << tm.tm_sec;
  } else {
    string_stream << tm.tm_sec;
  }
  return string_stream.str();
}

/**
 * Formats the date time into a string format that is file-name-safe.
 * @param tm
 * @return
 */
std::string TimeUtil::FormatDateTimeFileName(const tm& tm) {
  std::stringstream string_stream;
  static std::string file_name_field_delim = "_";

  // year
  string_stream << tm.tm_year + 1900 << file_name_field_delim;
  if (tm.tm_mon + 1 < 10) {
    string_stream << "0" << tm.tm_mon + 1 << file_name_field_delim;
  } else {
    string_stream << tm.tm_mon + 1 << file_name_field_delim;
  }
  if (tm.tm_mday < 10) {
    string_stream << "0" << tm.tm_mday << file_name_field_delim;
  } else {
    string_stream << tm.tm_mday << file_name_field_delim;
  }

  // time
  string_stream << tm.tm_hour << file_name_field_delim;
  if (tm.tm_min < 10) {
    string_stream << "0" << tm.tm_min << file_name_field_delim;
  } else {
    string_stream << tm.tm_min << file_name_field_delim;
  }
  if (tm.tm_sec < 10) {
    string_stream << "0" << tm.tm_sec;
  } else {
    string_stream << tm.tm_sec;
  }
  return string_stream.str();
}
std::string TimeUtil::GetDateTimeStr() {
  time_t t = time(nullptr);

#ifndef WIN32
  // todo: check warning here
  tm now;
  localtime_s(& now, & t);
  return FormatDateTime(now);
#elif defined(LINUX)
  tm now;
  localtime_r(& now, & t);
  return FormatDateTime(now);
#else
  tm* now = localtime( &t );
  return FormatDateTime( *now );
#endif
}
std::string TimeUtil::GetDateTimeFileNameStr() {
  time_t t = time(nullptr);
#ifndef WIN32
  tm now;
  localtime_s(& now, & t);
  return FormatDateTimeFileName(now);
#elif defined(LINUX)
  tm now;
  localtime_r(& now, & t);
  return FormatDateTimeFileName(now);

#else
  tm* now = localtime( &t );
  return FormatDateTimeFileName( *now );
#endif
}
