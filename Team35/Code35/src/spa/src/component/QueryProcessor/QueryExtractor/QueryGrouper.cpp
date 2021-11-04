#include <queue>
#include "QueryGrouper.h"
#include "QueryOptimizer.h"

/**
 * Gives a basic grouping for all the clauses, where all clauses are in the same group.
 * This method should be called when running the unoptimized version of the program.
 * @param clauses a vector of Clause pointers.
 * @param groups a vector of Group pointers, that is to be modified in-place.
 */
void QueryGrouper::BasicGroupClauses(std::vector<Clause*>* clauses, std::vector<Group*>* groups,
                                     std::unordered_map<std::string, Synonym*>* target_synonyms_map) {
  std::vector<std::string> tgt_synonyms_in_this_group;
  std::unordered_set<std::string> tgt_synonyms_seen;
  Group* g = new Group();
  for (auto cl : *clauses) {
    g->AddClauseToVector(cl);
    for (auto tgt_syn : cl->GetAllSynonymNamesOfClause()) {
      bool is_tgt_syn = target_synonyms_map->find(tgt_syn) != target_synonyms_map->end();
      bool is_tgt_seen = tgt_synonyms_seen.find(tgt_syn) != tgt_synonyms_seen.end();
      if (is_tgt_syn && !is_tgt_seen) {
        tgt_synonyms_seen.insert(tgt_syn);
        tgt_synonyms_in_this_group.push_back(tgt_syn);
      }
    }
  }
  groups->push_back(g);
  // populate group's metadata
  QueryGrouper::UpdateGroupMetadata(g, &tgt_synonyms_in_this_group, target_synonyms_map);
}

/**
 * Groups multiple queries that should be evaluated together, based on existence of common synonyms.
 */
void QueryGrouper::AdvancedGroupClauses(std::vector<WeightedClause*>* weighted_clauses,
                                        std::vector<WeightedGroup*>* weighted_groups,
                                        std::vector<std::pair<Synonym*, Attribute>>* target_syn_attrs_list,
                                        std::unordered_map<std::string, Synonym*>* target_synonyms_map,
                                        std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_clause_indices) {
  // First create groups for clauses containing target synonyms (and their 'neighbouring' synonyms).
  std::unordered_set<int> visited_clauses;
  std::unordered_set<std::string> visited_target_synonyms;
  std::unordered_set<std::string> visited_synonyms;
  for (auto p : *target_syn_attrs_list) {
    Synonym* tgt_syn = p.first;
    if (visited_target_synonyms.find(tgt_syn->GetName()) != visited_target_synonyms.end()) {
      continue;
    }
    // dfs starting from this target synonym
    QueryGrouper::DfsFromSynonym(target_synonyms_map, weighted_clauses, weighted_groups, tgt_syn->GetName(),
                                 &visited_clauses, &visited_target_synonyms, &visited_synonyms,
                                 map_of_syn_to_clause_indices);
  }
  // create groups for unvisited clauses
  for (int index_of_cl = 0; index_of_cl < weighted_clauses->size(); index_of_cl++) {
    if (visited_clauses.find(index_of_cl) != visited_clauses.end()) {
      continue;
    }
    std::vector<std::string> clause_syn_names = (*weighted_clauses)[index_of_cl]->clause->GetAllSynonymNamesOfClause();
    // if cl has no syns -> add to its own group; and add to visited
    if (clause_syn_names.size() == 0) {
      Group* g = new Group();
      WeightedGroup* w_g = new WeightedGroup(g);
      w_g->weighted_clauses.push_back((*weighted_clauses)[index_of_cl]);
      weighted_groups->push_back(w_g);
      visited_clauses.insert(index_of_cl);
    } else {
      // same dfs routine with either arg treated as 'tgt syn'
      QueryGrouper::DfsFromSynonym(target_synonyms_map, weighted_clauses, weighted_groups, clause_syn_names[0],
                                   &visited_clauses, &visited_target_synonyms, &visited_synonyms,
                                   map_of_syn_to_clause_indices);
    }
  }
}

void QueryGrouper::DfsFromSynonym(std::unordered_map<std::string, Synonym*>* tgt_synonyms_map,
                                    std::vector<WeightedClause*>* weighted_clauses,
                                    std::vector<WeightedGroup*>* weighted_groups, std::string tgt_syn,
                                    std::unordered_set<int>* visited_clauses,
                                    std::unordered_set<std::string>* visited_tgt_synonyms,
                                    std::unordered_set<std::string>* visited_synonyms,
                                    std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_clause_indices) {
  Group* g = new Group();
  WeightedGroup* w_g = new WeightedGroup(g);
  int subgroup_score = QueryOptimizer::GetSubgroupPenalty();
  std::vector<std::string> tgt_synonyms_in_this_group;
  bool is_starting_syn_a_target_syn = tgt_synonyms_map->find(tgt_syn) != tgt_synonyms_map->end();
  if (is_starting_syn_a_target_syn) {
    tgt_synonyms_in_this_group.push_back(tgt_syn);
    visited_tgt_synonyms->insert(tgt_syn);
  }
  visited_synonyms->insert(tgt_syn);
  // start queue-based dfs from the specified target synonym
  std::queue<std::string> q;
  q.push(tgt_syn);

  while (!q.empty()) {
    std::string curr_tgt_syn = q.front();
    q.pop();
    std::vector<int> cl_with_tgt_syns;
    if (map_of_syn_to_clause_indices->find(curr_tgt_syn) != map_of_syn_to_clause_indices->end()) {
      cl_with_tgt_syns = map_of_syn_to_clause_indices->at(curr_tgt_syn);
    }
    std::vector<int> subgroup;
    subgroup_score++;
    for (auto cl_idx : cl_with_tgt_syns) {
      if (visited_clauses->find(cl_idx) != visited_clauses->end()) {
        continue;
      }
      subgroup.push_back(cl_idx);

      visited_clauses->insert(cl_idx);
      std::vector<std::string> clause_syn_names = (*weighted_clauses)[cl_idx]->clause->GetAllSynonymNamesOfClause();
      for (std::string clause_syn_name : clause_syn_names) {
        if (visited_synonyms->find(clause_syn_name) != visited_synonyms->end()) {
          continue;
        }
        if (tgt_synonyms_map->find(clause_syn_name) != tgt_synonyms_map->end()) {
          visited_tgt_synonyms->insert(clause_syn_name);
          tgt_synonyms_in_this_group.push_back(clause_syn_name);
        }
        visited_synonyms->insert(clause_syn_name);
        q.push(clause_syn_name);
      }
      // save subgroup score and push each clause in subgroup into group
      WeightedClause* weighted_cl = weighted_clauses->at(cl_idx);
      weighted_cl->subgroup_penalty += subgroup_score;
      w_g->weighted_clauses.push_back(weighted_cl);
    }
  }
  weighted_groups->push_back(w_g);
  // populate group's metadata
  QueryGrouper::UpdateGroupMetadata(w_g, &tgt_synonyms_in_this_group, tgt_synonyms_map);
}

void QueryGrouper::UpdateGroupMetadata(Group* group, std::vector<std::string>* tgt_synonyms_in_grp,
                                         std::unordered_map<std::string, Synonym*>* tgt_syns_map) {
  for (auto item : *tgt_synonyms_in_grp) {
    group->AddSynToTargetSyns(tgt_syns_map->at(item));
  }
  group->UpdateHasTargetSynonymAttr();
}

void QueryGrouper::UpdateGroupMetadata(WeightedGroup* weighted_group, std::vector<std::string>* tgt_synonyms_in_grp,
                                       std::unordered_map<std::string, Synonym*>* tgt_syns_map) {
  for (auto item : *tgt_synonyms_in_grp) {
    weighted_group->group->AddSynToTargetSyns(tgt_syns_map->at(item));
  }
  weighted_group->group->UpdateHasTargetSynonymAttr();
}
