#ifndef AUTOTESTER_QUERYPARSER_H
#define AUTOTESTER_QUERYPARSER_H

#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
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
  std::list<Synonym*>& synonyms;
  std::vector<Synonym*>& target_synonyms_list;
  std::unordered_map<std::string, Synonym*>& target_synonyms_map;

  Token lookahead = Token("", TokenTag::kInvalid);
  QueryTokenizer tokenizer;
  Token Eat(TokenTag tokenType);

  void ParseQuery();
  void ParseSelect();
  void ParseSuchThat();
  void ParsePatternCond();
  void ParseAssignPattern();
  void ParseWhilePattern();
  void ParseIfPattern();
  void ParsePattern();
  void ParseAttrCompare();
  void ParseWith();
  void ParseDeclarations();
  void ParseTuple();
  void ParseTarget();
  void ParseDeclaration();
  std::pair<Synonym*, Attribute> ParseElem(bool is_first_pass);
  std::tuple<std::string, std::string, Synonym*, Attribute> ParseRef();
  Attribute ParseAttrName(Synonym* s);
  std::tuple<std::string, bool, bool> ParseStmtRef();
  std::pair<Token, bool> ParseEntRef(bool isPatternCl);
  std::tuple<std::string, bool, bool, bool> ParseStmtOrEntRef();
  std::pair<Clause*, bool> ParseRelRef();
  std::pair<std::string, bool> ParseExpressionSpec();
  std::vector<Token> ParseSynonyms();

  bool IsValidSynonym(Token token);
  bool IsValidSynonym(Token token, DesignEntity de);
 public:
  QueryParser(std::vector<Clause*>& clauses, std::list<Synonym*>& synonyms, std::vector<Synonym*>& target_synonyms_list,
              std::unordered_map<std::string, Synonym*>& target_synonyms_map, QueryTokenizer tokenizer) :
      clauses(clauses), synonyms(synonyms),
      target_synonyms_list(target_synonyms_list), target_synonyms_map(target_synonyms_map), tokenizer(tokenizer) {};
  void Parse();
  static Synonym* GetSynonymInfo(std::string syn_name, std::list<Synonym*>* synonyms);
};

#endif //AUTOTESTER_QUERYPARSER_H
