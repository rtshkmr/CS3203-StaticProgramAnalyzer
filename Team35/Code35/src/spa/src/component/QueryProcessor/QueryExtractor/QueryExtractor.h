#ifndef INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H
#define INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H

#include <string>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <component/QueryProcessor/types/Types.h>

/**
 * QueryExtractor is a facade that is in charge of tokenizing, parsing, validating, grouping queries.
 * before sending it off to QueryEvaluator.
 */
class QueryExtractor {
 private:
  std::string query;
  std::list<Group*> groups;
  std::list<Synonym*> synonyms;
  std::vector<Synonym*> target_synonyms;
  std::unordered_map<std::string, DesignEntity> target_synonyms_map;
  std::unordered_map<std::string, std::vector<int>> map_of_syn_to_clause_indices;
  static void PopulateSynAdjacencyList(std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_clause_indices,
                                       std::vector<Clause*>* clauses);
 public:
  explicit QueryExtractor(std::string* query) : query(* query) {};
  void ExtractQuery();
  std::list<Group*> GetGroupsList() { return groups; };
  std::list<Synonym*> GetSynonymsList() { return synonyms; }; // TODO: deprecate or convert to private.
  std::vector<Synonym*> GetTargetSynonymsList() { return target_synonyms; };
};

#endif //INC_21S1_CP_SPA_TEAM_35_QUERYEXTRACTOR_H
