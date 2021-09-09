/**
 * This header file contains the different data types used for this program.
 * Any data types used in this program should be immutable by default.
 */

#include <regex>
#include "string"

#ifndef AUTOTESTER_DATATYPE_H
#define AUTOTESTER_DATATYPE_H


bool ValidateName(std::string name);
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

  int getNum();

  bool operator<(const StatementNumber &other) const;

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
  LineNumber(int ln);

  int getNum();

  bool operator<(const LineNumber &other) const;

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
  ProcedureName(std::string pName);

  std::string getName();

  bool operator<(const ProcedureName &other) const;

  bool operator==(ProcedureName other) const;
};

/**
 * VariableName refers to the name for a variable.
 * This object checks and ensure that the received variable name is correct according to the lexical tokens
 */
class VariableName {
private:
  std::string name_;

public:
  VariableName(std::string vName);

  std::string getName();

  bool operator<(const VariableName &other) const;

  bool operator==(VariableName other) const;
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
  ConstantValue(std::string constant);

  int get();

  bool operator<(const ConstantValue &other) const;

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
  std::string GetTokenString();
  Token(std::string token_string, TokenTag token_tag);
  [[nodiscard]] TokenTag GetTokenTag() const;
  bool operator==(Token other);
  static TokenTag TagStringWithToken(std::string const& reference);
  static bool IsKeywordToken(Token token);
};

#endif //AUTOTESTER_DATATYPE_H