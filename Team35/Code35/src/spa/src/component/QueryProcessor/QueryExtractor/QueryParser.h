#ifndef AUTOTESTER_QUERYPARSER_H
#define AUTOTESTER_QUERYPARSER_H

#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include "QueryTokenizer.h"
#include <component/QueryProcessor/types/Types.h>

/**
 * QueryParser parses the query input from start to end into tokens, by calling QueryTokenizer.
 * Parsing is based on recursive-descent parsing that reflects the grammar rules of PQL.
 */
class QueryParser {
  private:
    std::unordered_set<std::string> synonyms_name_set;
    std::vector<Clause*>& clauses;
    std::list<Group*>& groups;
    std::list<Synonym>& synonyms;
    Synonym& target;

    Token lookahead = Token("", TokenTag::kInvalid);
    QueryTokenizer tokenizer;
    Token Eat(TokenTag tokenType);

    void ParseQuery();
    void ParseSelect();
    void ParseSuchThat();
    void ParsePattern();
    void ParseDeclarations();
    void ParseTarget();
    void ParseDeclaration();

    std::tuple<std::string, bool, bool> ParseStmtRef();
    std::pair<Clause*, bool> ParseRelRef();
    Token ParseEntRef(bool isPatternCl);
    std::pair<std::string, bool> ParseExpressionSpec();
    std::string ParseFactor();
    std::vector<Token> ParseSynonyms();

    bool IsValidSynonym(Token token);
    bool IsValidSynonym(Token token, DesignEntity de);
  public:
    QueryParser(std::vector<Clause*>& clauses, std::list<Group*>& groups, std::list<Synonym>& synonyms,
                Synonym& target, QueryTokenizer tokenizer) :
                clauses(clauses), groups(groups), synonyms(synonyms), target(target), tokenizer(tokenizer) {};
    void Parse();
    std::list<Group*> GetGroupsList() { return groups; };
    static Synonym GetSynonymInfo(std::string syn_name, std::list<Synonym>* synonyms);
};

#endif //AUTOTESTER_QUERYPARSER_H
