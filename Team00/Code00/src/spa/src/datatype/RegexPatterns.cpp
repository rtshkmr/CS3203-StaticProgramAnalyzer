//
// Created by mkrit on 6/9/2021.
//

#include <list>
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

//std::vector<std::string> RegexPatterns::GetSpecialDelimiters() {
//  std::vector special_delimiters{
//      "{", "(", ")", ";", "}", // important delims
//      "==", ">=", "<=", "!=", // double char comparison operators
//      ">",
//      "=", "<", // single char comparison operators
//      "+", "-", "*", "%", "/",
//  };
//  return special_delimiters;
//}

//std::list<std::string> RegexPatterns::GetSpecialDelimiters() {
//  std::vector special_delimiters{
//    "{", "(", ")", ";", "}", // important delims
//    "==", ">=", "<=", "!=", // double char comparison operators
//    ">",
//    "=", "<", // single char comparison operators
//    "+", "-", "*", "%", "/",
//    };
//  return special_delimiters;
//}

//std::string* RegexPatterns::GetSpecialDelimiters() {
//  std::string special_delimiters[] = {
//    "{", "(", ")", ";", "}", // important delims
//    "==", ">=", "<=", "!=", // double char comparison operators
//    ">",
//    "=", "<", // single char comparison operators
//    "+", "-", "*", "%", "/",
//    };
//  return special_delimiters;
//}

std::regex RegexPatterns::GetBinaryComparisonPattern() {
  return std::regex(R"(==|>|>=|<|<=|!=)");
}
std::regex RegexPatterns::GetNamePattern() {
  return std::regex(R"(^[[:alpha:]]+([0-9]+|[[:alpha:]]+)*)");
}
std::regex RegexPatterns::GetIntegerPattern() {
  return std::regex(R"([0-9]+)");
}
