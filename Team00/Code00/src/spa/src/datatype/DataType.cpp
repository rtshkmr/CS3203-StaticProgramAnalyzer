/**
 * This body file contains the different function implementation used for datatype.
 */

#include <string>
#include <stdexcept>
#include "DataType.h"

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
int StatementNumber::getNum() {
  return num_;
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
 * This ProcedureName constructor check if the procedure name is valid,
 *   and stores as a ProcedureName object.
 * @param pName [NOT NULL] The procedure name (in string) as extracted from SIMPLE program
 * @throws invalid_argument when an invalid procedure name is passed in.
 */
ProcedureName::ProcedureName(std::string pName) {
  //TODO: validate name syntax
  name_ = pName;
}

/**
 * Gets the name of the procedure.
 * @return [Not Null] The procedure name
 */
std::string ProcedureName::getName() {
  return name_;
}

/**
 * This VariableName constructor check if the variable name is valid,
 *   and stores as a VariableName object.
 * @param vName [NOT NULL] The variable name (in string) as extracted from SIMPLE program
 * @throws invalid_argument when an invalid variable name is passed in.
 */
VariableName::VariableName(std::string vName) {
  //TODO: validate name syntax
  name_ = vName;
}

/**
 * Gets the name of the variable.
 * @return [Not Null] The variable name
 */
std::string VariableName::getName() {
  return name_;
}

/**
 * This ConstantValue constructor check if the constant received is valid (valid = integer),
 *   and stores as a ConstantValue object.
 * @param constant [NOT NULL] The constant (in string) as extracted from SIMPLE program
 * @throws invalid_argument when a non-integer in passed in.
 */
ConstantValue::ConstantValue(std::string constant) {
  //TODO: check if constant is valid.
  value_ = stoi(constant);
}

/**
 * Gets the constant value.
 * @return [Not Null] The constant value
 */
int ConstantValue::get() {
  return value_;
}

/**
 * Constructor for token
 * @param token_string [NOT NULL] The string representation of the SIMPLE programme statement.
 * @param token_tag [NOT NULL] The enumerator object to the tag
 */

TokenTag Token::GetTokenTag() const {
  return this->token_tag_;
}
std::string Token::GetTokenString() {
  return this->token_string_;
}

Token::Token(string token_string, TokenTag token_tag) :
    token_tag_(token_tag) {
  token_string_ = std::move(token_string);
}

/**
 * Equality operator checks the string kept in the token todo: add the tag also
 * @param other
 * @return
 */
bool Token::operator==(Token other) {
  return this->GetTokenString() == other.GetTokenString();
}

bool Token::IsKeywordToken(Token token) {
  TokenTag tag = token.GetTokenTag();
  return tag == TokenTag::kProcedureKeyword
      || tag == TokenTag::kReadKeyword
      || tag == TokenTag::kPrintKeyword
      || tag == TokenTag::kCallKeyword
      || tag == TokenTag::kWhileKeyword
      || tag == TokenTag::kIfKeyword
      || tag == TokenTag::kWhileKeyword
      || tag == TokenTag::kThenKeyword;
}

TokenTag Token::TagStringWithToken(const string& reference) {
  regex fixed_keyword_pat(R"(procedure|read|print|call|while|if|then|else)");
  regex fixed_char_pat(R"(\{|\}|;|\(|\))"); // for braces, semicolon...
  regex binary_arithmetic_operator_pat(R"(\+|\-|\*|\/|%|=)"); // for math and comparator chars
  regex binary_comparison_operator_pat(R"(==|>|>=|<|<=|!=)"); // for math and comparator chars
  regex name_pat(R"(^[[:alpha:]]+([0-9]+|[[:alpha:]]+)*)"); // names, integers... todo: check alphanum
  regex integer_pat(R"([0-9]+)");

  // QQ: can it have a separation like that: "x > = 1"
  if (regex_match(reference, fixed_keyword_pat)) {
    // instead of having a header, we'll use these keywords for now
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
    // handle key characters:
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
  } else if (regex_match(reference, name_pat)) {
    return TokenTag::kName;
  } else if (regex_match(reference, integer_pat)) {
    return TokenTag::kInteger;
  } else {
    return TokenTag::kInvalid;
  }
}
