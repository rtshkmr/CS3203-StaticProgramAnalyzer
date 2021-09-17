/**
 * Tokenizer is responsible for converting the string format of a Source Statement into a list of lexical tokens,
 * as defined in the Concrete Grammar Syntax.
 */


#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_TOKENIZER_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_TOKENIZER_H_
#include <vector>
#include <string>
#include <datatype/DataType.h>

class Tokenizer {
 public:
  static std::vector<Token> CreateTokens(const std::string &statement_string);
 private:
  static Token* CreateToken(std::string string_token);
  static std::vector<std::string> SplitString(const std::string &delimiter,
                                              const std::string &input,
                                              bool retain_delimiter);
  static std::vector<std::string> SplitSubTokenStrings(std::string &string_token);
  static bool IsSpecialDelimiter(std::string &test_string);
  static bool IsWhiteSpace(std::string &test_string);
  static std::vector<std::string> SplitSentenceIntoStringTokens(const std::string& statement_string);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_TOKENIZER_H_
