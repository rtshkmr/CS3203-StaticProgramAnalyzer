/**
 * This implements logger functions which will be called by the various macros defined in the header file
 */
#include "Logger.h"

Logger spa_logger;
LoggerSettings::LoggerSettings() {
  this->filepath_ = "";
  this->filename_ = "Team35_SPA.log";
  this->can_overwrite_file_ = true;
  this->out_to_file_ = true;
  this->out_to_stdout_ = true;
  this->include_datetime_ = true;
  this->include_file_line_ = true;
}
// constructor:
Logger::Logger() {
  this->is_init_ = false;
}
Logger::~Logger() {
  if (this->is_init_) {
    this->TerminateLogger();
  }
}
void Logger::TerminateLogger() {
  if (this->is_init_) {
    this->ofstream_.close();
    this->is_init_ = false;
  }
}
/**
 * Initialises by creating a new or using an existing file stream and opening that ofstream up
 * @param settings
 * @return
 */
bool Logger::InitLogger(const LoggerSettings& settings) {
  this->logger_settings_ = settings;
  if (!this->is_init_) {
    if (this->logger_settings_.can_overwrite_file_) {
      this->ofstream_.open(this->logger_settings_.filepath_ + this->logger_settings_.filename_);
    } else { // open ofstream to new file
      this->ofstream_.open(
          this->logger_settings_.filepath_
              + TimeUtil::GetDateTimeFileNameStr() + "_"
              + this->logger_settings_.filename_);
    }

    if (this->ofstream_.is_open()) {
      this->is_init_ = true;
      return true; // successfully opened up a ofstream
    } else {
      return false;
    }
  }
  return true;
}
std::string Logger::AddPrefix(const std::string& file, const int line) const {
  std::string result = "";
  if (this->logger_settings_.include_datetime_) {
    result += TimeUtil::GetDateTimeStr();
    if (!this->logger_settings_.include_file_line_) {
      result += ": ";
    } else {
      result += ", ";
    }
  }
  if (this->logger_settings_.include_file_line_) {
    result += file + "(" + tostring(line) + "): ";
  }
  return result;
}
