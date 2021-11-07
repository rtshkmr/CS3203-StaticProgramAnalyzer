#ifndef AUTOTESTER_QUERYGROUPER_H
#define AUTOTESTER_QUERYGROUPER_H

#include <string>
#include <list>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "QueryOptimizer.h"
#include <component/QueryProcessor/types/Types.h>

class QueryGrouper {
 private:
  static void DfsFromSynonym(std::unordered_map<std::string, Synonym*>* target_synonyms_names,
                             std::vector<WeightedClause*>* clauses, std::vector<WeightedGroup*>* groups,
                             const std::string& tgt_syn, std::unordered_set<int>* visited_clauses,
                             std::unordered_set<std::string>* visited_tgt_synonyms,
                             std::unordered_set<std::string>* visited_synonyms,
                             std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_clause_indices);
  static void UpdateGroupMetadata(Group* group, std::vector<std::string>* tgt_synonyms_in_group,
                                  std::unordered_map<std::string, Synonym*>* tgt_synonyms_map);
  static void UpdateGroupMetadata(WeightedGroup* group, std::vector<std::string>* tgt_synonyms_in_group,
                                  std::unordered_map<std::string, Synonym*>* tgt_synonyms_map);
 public:
  static void AdvancedGroupClauses(std::vector<WeightedClause*>* clauses, std::vector<WeightedGroup*>* groups,
                                   std::vector<std::pair<Synonym*, Attribute>>* target_syn_attrs_list,
                                   std::unordered_map<std::string, Synonym*>* target_synonyms_map,
                                   std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_clause_indices);
  static void BasicGroupClauses(std::vector<Clause*>* clauses, std::vector<Group*>* groups,
                                std::unordered_map<std::string, Synonym*>* target_synonyms_map);
};

#endif //AUTOTESTER_QUERYGROUPER_H
