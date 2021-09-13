/**
 * The functions in this class exist to hide regex patterns and prevent modification.
 */
#include "RegexPatterns.h"

std::regex RegexPatterns::GetFixedKeywordPattern() {
  return std::regex(R"(procedure|read|print|call|while|if|then|else)");
}
std::regex RegexPatterns::GetFixedCharPattern() {
  return std::regex(R"(\{|\}|;|\(|\))");
}

std::regex RegexPatterns::GetBinaryArithmeticOperatorPattern() {

  return std::regex(R"(\+|\-|\*|\/|%|=)");
}

std::regex RegexPatterns::GetTermDelimiterPattern() {
  return std::regex(R"(\*|\/|%)");
}

std::regex RegexPatterns::GetExprDelimiterPattern() {
  return std::regex(R"(\+|\-)");
}

std::regex RegexPatterns::GetBinaryComparisonPattern() {
  return std::regex(R"(==|>|>=|<|<=|!=)");
}

std::regex RegexPatterns::GetBooleanOperatorPattern() {
  return std::regex(R"(&&|\|\||!)");
}
std::regex RegexPatterns::GetBinaryBooleanOperatorPattern() {
  return std::regex(R"(&&|\|\|)");
}

std::regex RegexPatterns::GetUnaryBooleanOperatorPattern() {
  return std::regex(R"(!)");
}
std::regex RegexPatterns::GetNamePattern() {
  return std::regex(R"(^[[:alpha:]]+([0-9]+|[[:alpha:]]+)*)");
}
std::regex RegexPatterns::GetIntegerPattern() {
  return std::regex(R"(^(0|[1-9][0-9]*)$)");
}
std::regex RegexPatterns::GetDesignEntityPattern() {
  return std::regex(R"(stmt|read|print|call|while|if|assign|variable|constant|procedure)");
};

