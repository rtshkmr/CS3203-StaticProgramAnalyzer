#ifndef AUTOTESTER_QUERYPARSER_H
#define AUTOTESTER_QUERYPARSER_H

#include <string>
#include <vector>
#include <list>
#include "QueryTokenizer.h"
#include <component/QueryProcessor/types/Types.h>

class QueryParser {
  private:
    std::list<Group>& groups;
    std::list<Synonym>& synonyms;
    Synonym& target;

    Token lookahead = Token("", TokenTag::kInvalid);
    QueryTokenizer tokenizer;

    Token eat(TokenTag tokenType);
    // helpers for recursive descent parsing
    void parse_query();
    void parse_declarations();
    std::tuple<std::string, bool, bool> parse_stmtRef();
    std::pair<Clause*, bool> parse_relRef();
    void parse_such_that();
    void parse_pattern();
    void parse_select();
    void GetTarget();
    void GetDeclaration();
    std::vector<Token> GetSynonyms();
  public:
  QueryParser(std::list<Group>& groups, std::list<Synonym>& synonyms, Synonym& target, QueryTokenizer tokenizer) :
              groups(groups), synonyms(synonyms), target(target), tokenizer(tokenizer) {};
  void parse();
};

#endif //AUTOTESTER_QUERYPARSER_H