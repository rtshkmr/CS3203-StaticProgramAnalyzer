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
  TokenTag GetPqlTokenType(std::string type);
 public:
  QueryTokenizer() {};
  void SetQueryString(std::string* query) { this->query = * query; };
  bool HasMoreTokens();
  Token GetNextToken();
};

#endif //AUTOTESTER_QUERYTOKENIZER_H
