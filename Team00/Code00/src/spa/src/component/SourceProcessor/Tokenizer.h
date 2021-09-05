/**
 * Tokenizer is responsible for converting the string format of a Source Statement into a list of lexical tokens,
 * as defined in the Concrete Grammar Syntax.
 */


#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_TOKENIZER_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_TOKENIZER_H_
#include <vector>
#include <string>
#include <datatype/DataType.h>
using namespace std;

class Tokenizer {
 public:
  static vector<Token> CreateTokens(const string& statement_string);
 private:
  static Token* CreateToken(string string_token);
  static vector<string> SplitString(const string& delimiter, const string& input, bool retain_delimiter);
  static vector<string> SplitSubTokens(string& string_token);
  static bool IsDelimiter(string& test_string);
  static bool IsWhiteSpace(string& test_string);
  static TokenTag TagTokenString(string token_string);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_TOKENIZER_H_
