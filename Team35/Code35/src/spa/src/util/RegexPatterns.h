/**
 * This class is a central store of regex patterns and other utilities.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_DATATYPE_REGEXPATTERNS_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_DATATYPE_REGEXPATTERNS_H_

#include <regex>

// todo: shift RegexPatterns to utils
class RegexPatterns {

 public:
  static std::regex GetFixedKeywordPattern();
  static std::regex GetFixedCharPattern();
  static std::regex GetBinaryArithmeticOperatorPattern();
  static std::regex GetBinaryComparisonPattern();
  static std::regex GetNamePattern();
  static std::regex GetIntegerPattern();
  static std::regex GetBooleanOperatorPattern();
  // for validator:
  static std::regex GetTermDelimiterPattern();
  static std::regex GetExprDelimiterPattern();
  // only NOT
  static std::regex GetBinaryBooleanOperatorPattern();
  static std::regex GetValidStatementTerminalToken();
  static std::regex GetIntegerPatternNonTerminating();
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_DATATYPE_REGEXPATTERNS_H_
