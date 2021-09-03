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
  Tokenizer();
  static vector<Token> CreateTokens(const string& statement_string);
 private:
  static Token CreateToken(const string& string_token);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_TOKENIZER_H_
