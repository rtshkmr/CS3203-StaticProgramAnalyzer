#ifndef AUTOTESTER_QUERYPARSER_H
#define AUTOTESTER_QUERYPARSER_H

#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include "QueryTokenizer.h"
#include <component/QueryProcessor/types/Types.h>

class QueryParser {
  private:
    std::unordered_set<std::string> synonyms_name_set;
    std::vector<Clause*>& clauses;
    std::list<Group*>& groups;
    std::list<Synonym>& synonyms;
    Synonym& target;

    Token lookahead = Token("", TokenTag::kInvalid);
    QueryTokenizer tokenizer;

    Token eat(TokenTag tokenType);
    // helpers for recursive descent parsing
    void ParseQuery();
    void GroupClauses();
    void ParseDeclarations();
    std::tuple<std::string, bool, bool> ParseStmtRef();
    std::pair<Clause*, bool> ParseRelRef();
    Token ParseEntRef(bool isPatternCl);
    std::string ParseFactor();
    std::pair<std::string, bool> ParseExpressionSpec();
    void ParseSuchThat();
    void ParsePattern();
    bool IsValidSynonym(Token token);
    bool IsValidSynonym(Token token, DesignEntity de);
    void ParseSelect();
    void GetTarget();
    void GetDeclaration();
    std::vector<Token> GetSynonyms();
  public:
    QueryParser(std::vector<Clause*>& clauses, std::list<Group*>& groups, std::list<Synonym>& synonyms,
                Synonym& target, QueryTokenizer tokenizer) :
                clauses(clauses), groups(groups), synonyms(synonyms), target(target), tokenizer(tokenizer) {};
    void Parse();
    std::list<Group*> GetGroupsList() { return groups; };
    static Synonym GetSynonymInfo(std::string syn_name, std::list<Synonym>* synonyms);
};

#endif //AUTOTESTER_QUERYPARSER_H
