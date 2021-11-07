/**
 * This header file contains the different data types used for this program.
 * Any data types used in this program should be immutable by default.
 */

#include <regex>
#include "string"

#ifndef AUTOTESTER_DATATYPE_H
#define AUTOTESTER_DATATYPE_H

bool ValidateName(const std::string& name);

/**
 * StatementNumber refers to the numbering of statements (program counter).
 * Note: The difference between StatementNumber and LineNumber is that a single line could have multiple
 *     statements, one statement, or partial statement.
 */
class StatementNumber {
 private:
  int num_;

 public:
  StatementNumber(int sn);

  int GetNum() const;

  bool operator<(const StatementNumber& other) const;

  bool operator==(StatementNumber other) const;
};

/**
 * LineNumber refers to the numbering of lines based on the input SIMPLE source code.
 * Note: The difference between StatementNumber and LineNumber is that a single line could have multiple
 *     statements, one statement, or partial statement.
 */
class LineNumber {
 private:
  int num_;

 public:
  explicit LineNumber(int ln);

  int GetNum() const;

  bool operator<(const LineNumber& other) const;

  bool operator==(LineNumber other) const;
};

/**
 * ProcedureName refers to the name for a procedure.
 * This object checks and ensure that the received procedure name is correct according to the lexical tokens
 */
class ProcedureName {
 private:
  std::string name_;

 public:
  ProcedureName(const std::string& pName);

  std::string GetName();

  bool operator<(const ProcedureName& other) const;

  bool operator==(const ProcedureName& other) const;
};

/**
 * VariableName refers to the name for a variable.
 * This object checks and ensure that the received variable name is correct according to the lexical tokens
 */
class VariableName {
 private:
  std::string name_;

 public:
  VariableName(const std::string& vName);

  std::string GetName() const;

  bool operator<(const VariableName& other) const;

  bool operator==(const VariableName& other) const;
};

/**
 * ConstantValue refers to a constant encountered in the program.
 * This object checks and ensure that the received constants is correct according to the lexical tokens,
 *   and based on the program rule limiting the number of digits within a constant.
 * Side note: constants does not have a name to identify them.
 */
class ConstantValue {
 private:
  int value_;

 public:
  ConstantValue(const std::string& constant);

  int GetValue() const;

  bool operator<(const ConstantValue& other) const;

  bool operator==(ConstantValue other) const;
};

enum class TokenTag {
  kInteger,
  kProcedureKeyword,
  kIfKeyword,
  kThenKeyword,
  kElseKeyword,
  kReadKeyword,
  kPrintKeyword,
  kCallKeyword,
  kWhileKeyword,
  kName,
  kBinaryArithmeticOperator, //  for binary math operations (=, +, -...)
  kBinaryComparisonOperator,
  kBooleanOperator, // &&, ||, ...
  kAssignmentOperator,
  kOpenBrace, // for containers procName{...}
  kCloseBrace,
  kOpenBracket,  // for if() and while()
  kCloseBracket,
  kSemicolon, // statement terminator
  kInvalid,
  kStringQuote, // "my_var"
  kComma,
  kSuchThat,
  kUnderscore,
  kTimes,
  kOpenKarat,
  kCloseKarat,
  kDot,
  kStmtHash,
  kProgLine,
  kEquals
};

/**
 * A Lexical Token represents a discrete unit within a particular source statement. It contains a string pointer to
 * the space-delimited string that it was originally created from as well as TokenTag that indicates the type of token it is.
 */
class Token {
 private:
  std::string token_string_;
  TokenTag token_tag_;

 public:
  std::string GetTokenString() const;
  Token(std::string token_string, TokenTag token_tag);
  TokenTag GetTokenTag() const;
  bool operator==(const Token& other) const;
  static TokenTag TagStringWithToken(std::string const& reference);
  static bool IsKeywordToken(const Token& token);
  static auto GetTokenMatchForwardIterator(const std::vector<Token>& tokens,
                                           const std::regex& desired_pattern,
                                           int left_idx,
                                           int right_idx);
  static auto GetTokenMatchForwardIterator(const std::vector<Token>& tokens,
                                           TokenTag target_token_tag,
                                           int left_idx,
                                           int right_idx);
  static auto GetTokenMatchReverseIterator(const std::vector<Token>& tokens,
                                           const std::regex& desired_pattern,
                                           int left_boundary_idx,
                                           int right_boundary_idx);
  static auto GetTokenMatchReverseIterator(const std::vector<Token>& tokens,
                                           TokenTag target_token_tag,
                                           int left_boundary_idx,
                                           int right_boundary_idx);
  static int GetFirstMatchingTokenIdx(const std::vector<Token>& tokens,
                                      TokenTag token_tag,
                                      int left_boundary_idx,
                                      int right_boundary_idx);

  static int GetFirstMatchingTokenIdx(const std::vector<Token>& tokens,
                                      const std::regex& desired_pattern,
                                      int left_boundary_idx,
                                      int right_boundary_idx);
  static int GetFirstMatchingTokenIdx(const std::vector<Token>& tokens,
                                                       const std::regex& desired_pattern);
  static int GetFirstMatchingTokenIdx(const std::vector<Token>& tokens, TokenTag target_token_tag);
  static int GetLastMatchingTokenIdx(const std::vector<Token>& tokens,
                                     const std::regex& desired_pattern,
                                     int left_boundary_idx,
                                     int right_boundary_idx);
  static int GetLastMatchingTokenIdx(const std::vector<Token>& tokens, TokenTag target_token_tag);
  static int CountTokens(std::vector<Token> tokens, TokenTag target_tag, std::string target_string);
  static int CountTokens(std::vector<Token> tokens, TokenTag target_tag);
};

#endif //AUTOTESTER_DATATYPE_H
