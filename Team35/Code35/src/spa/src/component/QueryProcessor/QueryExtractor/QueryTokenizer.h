#ifndef AUTOTESTER_QUERYTOKENIZER_H
#define AUTOTESTER_QUERYTOKENIZER_H

#include <string>
#include <vector>
#include <optional>
#include <component/QueryProcessor/types/Types.h>
#include <datatype/DataType.h>

/**
 * QueryTokenizer is the component that tokenizes the input pql query.
 */
class QueryTokenizer {
 private:
  std::string query;
  std::int16_t cursor = 0;
  static TokenTag GetPqlTokenType(const std::string& type);
  static std::pair<bool, Token> EvaluateExactTokenMatch(const std::string& curr_string);
 public:
  QueryTokenizer() = default;;
  void SetQueryString(std::string* input_query) { this->query = * input_query; };
  bool HasMoreTokens();
  Token GetNextToken();
  std::string SkipTokenizerTillStringQuoteDelimiter();
};

#endif //AUTOTESTER_QUERYTOKENIZER_H
