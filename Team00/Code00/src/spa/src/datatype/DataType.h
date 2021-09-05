/**
 * This header file contains the different data types used for this program.
 * Any data types used in this program should be immutable by default.
 */

#include <regex>
#include "string"

#ifndef AUTOTESTER_DATATYPE_H
#define AUTOTESTER_DATATYPE_H

using namespace std;

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
};

// header:
enum class StatementTypeTag {
  kReadStatement,
  kPrintStatement,
  kCallStatement,
  kWhileStatement,
  kIfStatement,
  kAssignStatement,
};

enum class TokenTag {
  // todo: add fixed set of tags for token from the Concrete Grammar
  //       these tags will determine the rule-set that is used by the CGV to validate syntax
  kInteger,
  kKeyword, // for SIMPLE keywords like call, print...
  kProcedureKeyword,
  kIfKeyword,
  kReadKeyword,
  kPrintKeyword,
  kCallKeyword,
  kWhileKeyword,
  kName,
  kBinaryArithmeticOperator, //  for binary math operations (=, +, -...)
  kBinaryComparisonOperator,
  kAssignmentOperator,
  kOpenBrace, // for containers procName{...}
  kCloseBrace,
  kOpenBracket,  // for if() and while()
  kCloseBracket,
  kSemicolon, // statement terminator
  kInvalid,
};



// QQ : is it better to make Token into a struct? was going to encapsulate other methods into it
/**
 * A Lexical Token represents a discrete unit within a particular source statement. It contains a string pointer to
 * the space-delimited string that it was originally created from as well as TokenTag that indicates the type of token it is.
 */
class Token {
 private:
  string token_string_;
 public:
  string GetTokenString();
  TokenTag token_tag_; // fixme: make this private
  Token(string token_string, TokenTag token_tag);
  [[nodiscard]] TokenTag GetTokenTag() const;
  bool operator==(Token other);
  static TokenTag TagStringWithToken(string const& reference);
  static bool IsKeywordToken(Token token);
};

//// psub should call for a TokenizedStatement
//[[maybe_unused]] typedef struct TokenizedStatement {
//  std::list<Token> tokens;
//  StatementTypeTag header;
//} TokenizedStatement;

#endif //AUTOTESTER_DATATYPE_H
