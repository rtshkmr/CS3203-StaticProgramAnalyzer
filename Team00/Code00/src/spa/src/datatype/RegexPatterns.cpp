//
// Created by mkrit on 6/9/2021.
//

#include "RegexPatterns.h"


// hidden within accessors to prevent accidental modification
std::regex RegexPatterns::GetFixedKeywordPattern() {
  return std::regex(R"(procedure|read|print|call|while|if|then|else)");
}
std::regex RegexPatterns::GetFixedCharPattern() {
  return std::regex(R"(\{|\}|;|\(|\))");
}
std::regex RegexPatterns::GetBinaryArithmeticOperatorPattern() {
  return std::regex(R"(\+|\-|\*|\/|%|=|==|>|>=|<|<=|!=)");
}
std::regex RegexPatterns::GetBinaryComparisonPattern() {
  return std::regex(R"(==|>|>=|<|<=|!=)");
}
std::regex RegexPatterns::GetNamePattern() {
  return std::regex(R"(^[[:alpha:]]+([0-9]+|[[:alpha:]]+)*)");
}
std::regex RegexPatterns::GetIntegerPattern() {
  return std::regex(R"([0-9]+)");
}
