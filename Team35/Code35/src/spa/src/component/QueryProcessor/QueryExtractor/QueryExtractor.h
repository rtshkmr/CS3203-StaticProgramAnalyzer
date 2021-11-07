#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H

#include <string>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <component/QueryProcessor/types/Types.h>

/**
 * QueryExtractor is a facade that is in charge of tokenizing, parsing, validating, grouping queries.
 * before sending it off to QueryEvaluator.
 */
class QueryExtractor {
 private:
  std::string query;
  std::vector<Group*> groups;
  std::list<Synonym*> synonyms;
  std::vector<std::pair<Synonym*, Attribute>> target_syn_attrs;
  std::unordered_map<std::string, Synonym*> target_synonyms_map;
  bool was_query_boolean = false;
 public:
  explicit QueryExtractor(std::string* query) : query(* query) {};
  void ExtractQuery();
  void ExtractQuery(bool are_optimizations_enabled);
  std::vector<Group*> GetGroupsList() { return groups; };
  std::list<Synonym*> GetSynonymsList() { return synonyms; };
  std::vector<std::pair<Synonym*, Attribute>> GetTargetSynAttrPairs();
  bool WasQueryBoolean() { return was_query_boolean; };
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H
