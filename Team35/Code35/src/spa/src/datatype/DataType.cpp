/**
 * This body file contains the different function implementation used for datatype.
 */

#include <string>
#include <stdexcept>
#include "DataType.h"
#include <datatype/RegexPatterns.h>
#include <cassert>
#include <exception/SpaException.h>
#include "../util/Utility.h"

/**
 * This method checks if the given string as in the correct name syntax.
 * @param name The name to be checked.
 * @return true if it is in the correct name syntax; false otherwise.
 */
bool ValidateName(const std::string& name) {
  std::regex name_regex("^[A-Za-z][A-Za-z0-9]*$");
  return std::regex_search(name, name_regex);
}

/**
 * This StatementNumber constructor check if the statement number input is valid,
 *   and stores as a StatementNumber object.
 * @param sn [NOT NULL] The statement number
 * @throws invalid_argument when a negative integer is passed in.
 */
StatementNumber::StatementNumber(int sn) {
  if (sn < 1)
    throw std::invalid_argument("Statement Number should not be negative.");
  num_ = sn;
}

/**
 * Gets the statement number.
 * @return [Not Null] The statement number
 */
int StatementNumber::GetNum() const {
  return num_;
}

/**
 * Performs a comparison between the current StatementNumber with the input StatementNumber.
 * This method is similar to Java's Comparator, thus by creating this method, vectors<StatementNumber> can be sorted.
 * @param other The StatementNumber object to compare to
 * @return true if this.num_ is smaller than other.num_
 */
bool StatementNumber::operator<(const StatementNumber& other) const {
  return num_ < other.num_;
}

/**
 * Performs an equality comparison between the current StatementNumber with the input StatementNumber.
 * This method is similar to Java's equals().
 * @param other The StatementNumber object to compare to
 * @return true if this.num_ is equal to other.num_
 */
bool StatementNumber::operator==(StatementNumber other) const {
  return this->num_ == other.num_;
}

/**
 * This LineNumber constructor check if the line number input is valid,
 *   and stores as a LineNumber object.
 * @param ln [NOT NULL] The line number
 * @throws invalid_argument when a negative integer is passed in.
 */
LineNumber::LineNumber(int ln) {
  if (ln < 1)
    throw std::invalid_argument("Line Number should not be negative.");
  num_ = ln;
}

/**
 * Gets the line number.
 * @return [Not Null] The line number
 */
int LineNumber::getNum() {
  return num_;
}

/**
 * Performs a comparison between the current LineNumber with the input LineNumber.
 * This method is similar to Java's Comparator, thus by creating this method, vectors<LineNumber> can be sorted.
 * @param other The LineNumber object to compare to
 * @return true if this.num_ is smaller than other.num_
 */
bool LineNumber::operator<(const LineNumber& other) const {
  return num_ < other.num_;
}

/**
 * Performs an equality comparison between the current LineNumber with the input LineNumber.
 * This method is similar to Java's equals().
 * @param other The LineNumber object to compare to
 * @return true if this.num_ is equal to other.num_
 */
bool LineNumber::operator==(LineNumber other) const {
  return this->num_ == other.num_;
}

/**
 * This ProcedureName constructor check if the procedure name is valid,
 *   and stores as a ProcedureName object.
 * @param pName [NOT NULL] The procedure name (in string) as extracted from SIMPLE program
 * @throws invalid_argument when an invalid procedure name is passed in.
 */
ProcedureName::ProcedureName(std::string pName) {
  if (ValidateName(pName)) {
    name_ = pName;
  } else {
    throw std::invalid_argument("Invalid Procedure Name given.");
  }
}

/**
 * Gets the name of the procedure.
 * @return [Not Null] The procedure name
 */
std::string ProcedureName::getName() {
  return name_;
}

/**
 * Performs a comparison between the current ProcedureName with the input ProcedureName.
 * This method is similar to Java's Comparator, thus by creating this method, vectors<ProcedureName> can be sorted.
 * @param other The ProcedureName object to compare to
 * @return true if this.name_ is strictly smaller than other.name_. Note that if both strings are equivalent,
 *    it will return false;
 */
bool ProcedureName::operator<(const ProcedureName& other) const {
  return (strcmp(this->name_.data(), other.name_.data()) < 0);
}

/**
 * Performs an equality comparison between the current ProcedureName with the input ProcedureName.
 * This method is similar to Java's equals().
 * @param other The ProcedureName object to compare to
 * @return true if this.name_ is equal to other.name_
 */
bool ProcedureName::operator==(ProcedureName other) const {
  return this->name_ == other.name_;
}

/**
 * This VariableName constructor check if the variable name is valid,
 *   and stores as a VariableName object.
 * @param vName [NOT NULL] The variable name (in string) as extracted from SIMPLE program
 * @throws invalid_argument when an invalid variable name is passed in.
 */
VariableName::VariableName(std::string vName) {
  if (ValidateName(vName)) {
    name_ = vName;
  } else {
    throw std::invalid_argument("Invalid Variable Name given.");
  }
}

/**
 * Gets the name of the variable.
 * @return [Not Null] The variable name
 */
std::string VariableName::GetName() const{
  return name_;
}

/**
 * Performs a comparison between the current VariableName with the input VariableName.
 * This method is similar to Java's Comparator, thus by creating this method, vectors<VariableName> can be sorted.
 * @param other The VariableName object to compare to
 * @return true if this.name_ is strictly smaller than other.name_. Note that if both strings are equivalent,
 *    it will return false;
 */
bool VariableName::operator<(const VariableName& other) const {
  return (strcmp(this->name_.data(), other.name_.data()) < 0);
}

/**
 * Performs an equality comparison between the current VariableName with the input VariableName.
 * This method is similar to Java's equals().
 * @param other The VariableName object to compare to
 * @return true if this.name_ is equal to other.name_
 */
bool VariableName::operator==(const VariableName& other) const {
  return this->name_ == other.name_;
}

/**
 * This ConstantValue constructor check if the constant received is valid (valid = integer up to +- 2,147,483,647),
 *   and stores as a ConstantValue object.
 * @param constant [NOT NULL] The constant (in string) as extracted from SIMPLE program
 * @throws SyntaxException when a non-integer in passed in or when integers that had exceeded the range.
 */
ConstantValue::ConstantValue(const std::string& constant) {
  value_ = Utility::ConvertStringToInt(constant);
}

/**
 * Gets the constant value.
 * @return [Not Null] The constant value
 */
int ConstantValue::Get() {
  return value_;
}

/**
 * Performs a comparison between the current ConstantValue with the input ConstantValue.
 * This method is similar to Java's Comparator, thus by creating this method, vectors<ConstantValue> can be sorted.
 * @param other The ConstantValue object to compare to
 * @return true if this.value_ is smaller than other.value_
 */
bool ConstantValue::operator<(const ConstantValue& other) const {
  return value_ < other.value_;
}

/**
 * Performs an equality comparison between the current ConstantValue with the input ConstantValue.
 * This method is similar to Java's equals().
 * @param other The ConstantValue object to compare to
 * @return true if this.name_ is equal to other.name_
 */
bool ConstantValue::operator==(ConstantValue other) const {
  return this->value_ == other.value_;
}

/**
 * Constructor for token
 * @param token_string [NOT NULL] The string representation of the SIMPLE programme statement.
 * @param token_tag [NOT NULL] The enumerator object to the tag
 */

TokenTag Token::GetTokenTag() const {
  return this->token_tag_;
}
std::string Token::GetTokenString() const {
  return this->token_string_;
}

Token::Token(std::string token_string, TokenTag token_tag) :
    token_tag_(token_tag) {
  token_string_ = std::move(token_string);
}

/**
 * Equality operator checks the string kept in the token
 * @param other
 * @return
 */
bool Token::operator==(const Token& other) const {
  return this->GetTokenString() == other.GetTokenString()
      && this->GetTokenTag() == other.GetTokenTag();
}

bool Token::IsKeywordToken(const Token& token) {
  TokenTag tag = token.GetTokenTag();
  return tag == TokenTag::kProcedureKeyword
      || tag == TokenTag::kReadKeyword
      || tag == TokenTag::kPrintKeyword
      || tag == TokenTag::kCallKeyword
      || tag == TokenTag::kWhileKeyword
      || tag == TokenTag::kIfKeyword
      || tag == TokenTag::kElseKeyword
      || tag == TokenTag::kThenKeyword;
}

/**
 * Tags a reference string with a TokenTag by comparing with a various regex that represent a fixed set of
 * concrete syntax grammar rules. As a side-effect, any reference string that doesn't match the given regex
 * patterns would violate the Concrete Grammar Syntax for SIMPLE and therefore would be tagged with a kInvalid tag for
 * further error handling
 *
 * @param reference  a string that will be tagged
 * @return a TokenTag enum object
 */
TokenTag Token::TagStringWithToken(const std::string& reference) {

  std::regex fixed_keyword_pat = RegexPatterns::GetFixedKeywordPattern();
  std::regex fixed_char_pat = RegexPatterns::GetFixedCharPattern();
  std::regex binary_arithmetic_operator_pat = RegexPatterns::GetBinaryArithmeticOperatorPattern();
  std::regex binary_comparison_operator_pat = RegexPatterns::GetBinaryComparisonPattern();
  std::regex boolean_operator_pat = RegexPatterns::GetBooleanOperatorPattern();
  std::regex name_pat = RegexPatterns::GetNamePattern();
  std::regex integer_pat = RegexPatterns::GetIntegerPattern();

  if (regex_match(reference, fixed_keyword_pat)) {
    if (reference == "procedure") {
      return TokenTag::kProcedureKeyword;
    } else if (reference == "read") {
      return TokenTag::kReadKeyword;
    } else if (reference == "print") {
      return TokenTag::kPrintKeyword;
    } else if (reference == "call") {
      return TokenTag::kCallKeyword;
    } else if (reference == "while") {
      return TokenTag::kWhileKeyword;
    } else if (reference == "if") {
      return TokenTag::kIfKeyword;
    } else if (reference == "then") {
      return TokenTag::kThenKeyword;
    } else if (reference == "else") {
      return TokenTag::kElseKeyword;
    } else {
      return TokenTag::kInvalid;
    }
  } else if (regex_match(reference, fixed_char_pat)) {
    if (reference == "{") {
      return TokenTag::kOpenBrace;
    } else if (reference == "}") {
      return TokenTag::kCloseBrace;
    } else if (reference == ";") {
      return TokenTag::kSemicolon;
    } else if (reference == "(") {
      return TokenTag::kOpenBracket;
    } else if (reference == ")") {
      return TokenTag::kCloseBracket;
    } else {
      return TokenTag::kInvalid;
    }
  } else if (regex_match(reference, binary_arithmetic_operator_pat)) {
    if (reference == "=") {
      return TokenTag::kAssignmentOperator;
    } else {
      return TokenTag::kBinaryArithmeticOperator;
    }
  } else if (regex_match(reference, binary_comparison_operator_pat)) {
    return TokenTag::kBinaryComparisonOperator;
  } else if (regex_match(reference, boolean_operator_pat)) {
    return TokenTag::kBooleanOperator;
  } else if (regex_match(reference, name_pat)) {
    return TokenTag::kName;
  } else if (regex_match(reference, integer_pat)) {
    return TokenTag::kInteger;
  } else {
    return TokenTag::kInvalid;
  }
}

/**
 * Counts the number of tokens in a vector that matches a given target TokenTag
 * @param tokens tokens to count in
 * @param target_tag the target tag to meet
 * @return
 */
int Token::CountTokens(std::vector<Token> tokens, TokenTag target_tag) {
  int count = std::count_if(tokens.begin(), tokens.end(), [&target_tag](Token t) {
    return t.GetTokenTag() == target_tag;
  });
  return count;
}

/**
 * Counts the number of tokens in a vector that matches a given target TokenTag and string
 * @param tokens tokens to count in
 * @param target_tag the target tag to meet
 * @param target_string the string target to meet
 * @return
 */
int Token::CountTokens(std::vector<Token> tokens, TokenTag target_tag, std::string target_string) {
  int count = std::count_if(tokens.begin(), tokens.end(), [&target_tag, &target_string](Token t) {
    return t.GetTokenTag() == target_tag
        && t.GetTokenString() == target_string;
  });
  return count;
}

/**
 *  Returns an iterator for the tokens vector for the token that matches the regex for the desired patterns, this
 *  is to help identify where to recurse into.
 * @param tokens
 * @param desired_pattern
 * @param left_idx
 * @param right_idx
 * @return
 */
auto Token::GetTokenMatchForwardIterator(const std::vector<Token>& tokens,
                                         const std::regex& desired_pattern,
                                         int left_idx,
                                         int right_idx) {
  auto forward_iterator = std::find_if(tokens.begin() + left_idx,
                                       tokens.begin()
                                           + right_idx,
                                       [&desired_pattern](const Token& elem) {
                                         std::string current = elem.GetTokenString();
                                         bool matches_target_pattern = std::regex_match(current, desired_pattern);
                                         return matches_target_pattern;
                                       });
  return forward_iterator;
}
auto Token::GetTokenMatchForwardIterator(const std::vector<Token>& tokens,
                                         TokenTag target_token_tag,
                                         int left_idx,
                                         int right_idx) {
  auto forward_iterator = std::find_if(tokens.begin() + left_idx,
                                       tokens.begin() + right_idx,
                                       [&target_token_tag](const Token& elem) {
                                         TokenTag current_tag = elem.GetTokenTag();
                                         bool matches_target_token = current_tag == target_token_tag;
                                         return matches_target_token;
                                       });
  return forward_iterator;
}

// reverse iterator use:
/**
 * Takes in a range in the form of left and right idx which refers to the actual vector of tokens and within this,
 * finds the first token FROM THE END OF THE VECTOR that matches a desired pattern.
 * @param tokens
 * @param desired_pattern
 * @param left_boundary_idx
 * @param right_boundary_idx
 * @return  the reverse iterator representing this first token
 */
auto Token::GetTokenMatchReverseIterator(const std::vector<Token>& tokens,
                                         const std::regex& desired_pattern,
                                         int left_boundary_idx,
                                         int right_boundary_idx) {
  // from the given actual indices, Get respective reverse iterators for the range
  auto rBeginning = tokens.crbegin() + ((tokens.size() - 1) - right_boundary_idx);
  auto rEnding = tokens.crend() - left_boundary_idx;
  // nb: find_if checks within a half-open range but we want inclusive behaviour:
  auto reverse_iterator = std::find_if(rBeginning,
                                       rEnding - 1,
                                       [&desired_pattern](const Token& elem) {
                                         std::string current = elem.GetTokenString();
                                         bool matches_target_pattern = std::regex_match(current, desired_pattern);
                                         return matches_target_pattern;
                                       });
  return reverse_iterator;
}
auto Token::GetTokenMatchReverseIterator(const std::vector<Token>& tokens,
                                         TokenTag target_token_tag,
                                         int left_boundary_idx,
                                         int right_boundary_idx) {
  // from the given actual indices, Get respective reverse iterators for the range
  auto rBeginning = tokens.crbegin() + ((tokens.size() - 1) - right_boundary_idx);
  auto rEnding = tokens.crend() - left_boundary_idx;
  // nb: find_if checks within a half-open range but we want inclusive behaviour:
  auto reverse_iterator = std::find_if(rBeginning,
                                       rEnding - 1,
                                       [&target_token_tag](const Token& elem) {
                                         TokenTag current_tag = elem.GetTokenTag();
                                         bool matches_target_token = current_tag == target_token_tag;
                                         return matches_target_token;
                                       });
  return reverse_iterator;
}
/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the first token that matches
 * the desired target token tag.
 * @param tokens
 * @param token_tag
 * @param left_boundary_idx
 * @param right_boundary_idx
 * @return
 */
int Token::GetFirstMatchingTokenIdx(const std::vector<Token>& tokens,
                                    TokenTag token_tag,
                                    int left_boundary_idx,
                                    int right_boundary_idx) {
  auto delim_iterator = GetTokenMatchForwardIterator(tokens,
                                                     token_tag,
                                                     left_boundary_idx,
                                                     right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator);
  return delim_idx;
}
/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the first token that matches
 * the desired regex pattern.
 * @param tokens
 * @param desired_pattern
 * @param left_boundary_idx (inclusive boundary)
 * @param right_boundary_idx (inclusive boundary)
 * @return
 */
int Token::GetFirstMatchingTokenIdx(const std::vector<Token>& tokens,
                                    const std::regex& desired_pattern,
                                    int left_boundary_idx,
                                    int right_boundary_idx) {
  auto delim_iterator = GetTokenMatchForwardIterator(tokens,
                                                     desired_pattern,
                                                     left_boundary_idx,
                                                     right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator);
  return delim_idx;
}
/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the last token that matches
 * the desired regex pattern.
 * @param tokens
 * @param desired_pattern
 * @param left_boundary_idx (inclusive boundary)
 * @param right_boundary_idx (inclusive boundary)
 * @return
 */
int Token::GetLastMatchingTokenIdx(const std::vector<Token>& tokens,
                                   const std::regex& desired_pattern,
                                   int left_boundary_idx,
                                   int right_boundary_idx) {
  auto delim_iterator = GetTokenMatchReverseIterator(tokens,
                                                     desired_pattern,
                                                     left_boundary_idx,
                                                     right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}
/**
 * For a given vector of tokens and a boundary of indices to inspect, returns the index of the last token that matches
 * the target token tag.
 * @param tokens
 * @param target_token_tag
 * @param left_boundary_idx (inclusive boundary)
 * @param right_boundary_idx (inclusive boundary)
 * @return
 */
int Token::GetLastMatchingTokenIdx(const std::vector<Token>& tokens,
                                   TokenTag target_token_tag,
                                   int left_boundary_idx,
                                   int right_boundary_idx) {
  auto delim_iterator = GetTokenMatchReverseIterator(tokens,
                                                     target_token_tag,
                                                     left_boundary_idx,
                                                     right_boundary_idx);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}
/**
 * Returns first matching token based on the desired regex pattern within the entire vector of tokens
 * @param tokens
 * @param desired_pattern
 * @return
 */
int Token::GetFirstMatchingTokenIdx(const std::vector<Token>& tokens, const std::regex& desired_pattern) {
  assert(!tokens.empty());
  auto delim_iterator = GetTokenMatchForwardIterator(tokens,
                                                     desired_pattern,
                                                     0,
                                                     tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator);
  return delim_idx;
}
/**
 * Returns first matching token based on the desired regex pattern within the entire vector of tokens
 * @param tokens
 * @param target_token_tag
 * @return
 */
int Token::GetFirstMatchingTokenIdx(const std::vector<Token>& tokens, TokenTag target_token_tag) {
  assert(!tokens.empty());
  auto delim_iterator = GetTokenMatchForwardIterator(tokens,
                                                     target_token_tag,
                                                     0,
                                                     tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator);
  return delim_idx;
}
/**
 * For a given vector of tokens, returns the index of the last token that matches
 * the desired regex pattern in the entire vector.
 * @param tokens
 * @param desired_pattern
 * @return
 */
int Token::GetLastMatchingTokenIdx(const std::vector<Token>& tokens, const std::regex& desired_pattern) {
  auto delim_iterator = GetTokenMatchReverseIterator(tokens,
                                                     desired_pattern,
                                                     0,
                                                     tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}

/**
 * For a given vector of tokens, returns the index of the last token that matches
 * the desired regex pattern in the entire vector.
 * @param tokens
 * @param desired_pattern
 * @return
 */
int Token::GetLastMatchingTokenIdx(const std::vector<Token>& tokens, TokenTag target_token_tag) {
  auto delim_iterator = GetTokenMatchReverseIterator(tokens,
                                                     target_token_tag,
                                                     0,
                                                     tokens.size() - 1);
  int delim_idx = std::distance(tokens.begin(), delim_iterator.base()) - 1;
  return delim_idx;
}
