#ifndef AUTOTESTER_QUERYPARSER_H
#define AUTOTESTER_QUERYPARSER_H

#include <string>
#include <vector>
#include <list>
#include "QueryTokenizer.h"
#include <component/QueryProcessor/types/Types.h>

class QueryParser {
  private:
    std::vector<Clause*>& clauses;
    std::list<Group>& groups;
    std::list<Synonym>& synonyms;
    Synonym& target;

    Token lookahead = Token("", TokenTag::kInvalid);
    QueryTokenizer tokenizer;

    Token eat(TokenTag tokenType);
    // helpers for recursive descent parsing
    void parse_query();
    void group_clauses();
    void parse_declarations();
    std::tuple<std::string, bool, bool> parse_stmtRef();
    std::pair<Clause*, bool> parse_relRef();
    Token parse_entRef(bool isPatternCl);
    std::string parse_factor();
    std::pair<std::string, bool> parse_expressionSpec();
    void parse_such_that();
    void parse_pattern();
    bool is_valid_synonym(Token token);
    bool is_valid_synonym(Token token, DesignEntity de);
    void parse_select();
    void GetTarget();
    void GetDeclaration();
    std::vector<Token> GetSynonyms();
  public:
  QueryParser(std::vector<Clause*>& clauses, std::list<Group>& groups, std::list<Synonym>& synonyms,
              Synonym& target, QueryTokenizer tokenizer) :
              clauses(clauses), groups(groups), synonyms(synonyms), target(target), tokenizer(tokenizer) {};
  void parse();
};

#endif //AUTOTESTER_QUERYPARSER_H
