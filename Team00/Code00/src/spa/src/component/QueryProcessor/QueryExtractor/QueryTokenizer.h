#ifndef AUTOTESTER_QUERYTOKENIZER_H
#define AUTOTESTER_QUERYTOKENIZER_H

#include <string>
#include <vector>
#include <optional>
#include <component/QueryProcessor/types/Types.h>
#include <datatype/DataType.h>

class QueryTokenizer {
  private:
    std::string query;
    std::int16_t cursor = 0;
    TokenTag getPqlTokenType(std::string type);
  public:
    QueryTokenizer() {};
    void setQueryString(std::string* query) { this->query = *query; };
    bool hasMoreTokens();
    Token getNextToken();
};

#endif //AUTOTESTER_QUERYTOKENIZER_H
