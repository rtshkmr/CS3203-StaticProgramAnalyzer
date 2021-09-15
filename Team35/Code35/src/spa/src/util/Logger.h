/**
 * This file implements a Logger class and a settings class that the Logger relies on.
 */
#ifndef AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_LOGGER_H_
#define AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_LOGGER_H_
#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <util/TimeUtil.h>
#include <util/util.h>

class Logger;
extern Logger spa_logger;
#define LoggerInit(logger_settings) spa_logger.InitLogger(logger_settings);
#define LoggerTerminate() spa_logger.TerminateLogger();
#define LOG (spa_logger << spa_logger.AddPrefix(__FILE__, __LINE__));

class LoggerSettings {
 public:
  std::string filepath_;
  std::string filename_;
  bool can_overwrite_file_;
  bool out_to_file_;
  bool out_to_stdout_;
  bool include_datetime_;
  bool include_file_line_;
  // default constructor:
  LoggerSettings();
};



class Logger {
 private:
  bool is_init_;
  LoggerSettings logger_settings_;
  std::ofstream ofstream_;

 public:
  Logger();
  ~Logger();

  void TerminateLogger();
  bool InitLogger(const LoggerSettings& settings);
  template<typename T>
  Logger& operator<<(T t);
  Logger& operator<<(std::ostream& (* fun)(std::ostream&));
  [[nodiscard]] std::string AddPrefix(const std::string& file, int line) const;
};

/**
 * This is an overloaded operator to support standard data types being
 * output, depending on the setting either or (or both) to file and stdout
 * @tparam T
 * @param t
 * @return
 */
template<typename T>
inline Logger& Logger::operator<<(T t) {
  if ( this->is_init_ )
  {
    if ( this->logger_settings_.out_to_file_ ) this->ofstream_ << t;
    if ( this->logger_settings_.out_to_stdout_ ) std::cout << t;
  }
  return *this;
}

/**
 *  This  is an overloaded operator to support adding of std::endl.
 */
inline Logger& Logger::operator<<(std::ostream& (* fun)(std::ostream&)) {
  if ( this->is_init_ )
  {
    if ( this->logger_settings_.out_to_file_ ) this->ofstream_ << std::endl;
    if ( this->logger_settings_.out_to_stdout_ ) std::cout << std::endl;
  }
  return *this;
}
#endif //AUTOTESTER_TEAM35_CODE35_SRC_SPA_SRC_UTIL_LOGGER_H_
