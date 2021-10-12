#include <queue>
#include "QueryGrouper.h"
#include <component/QueryProcessor/types/Exceptions.h>

/**
 * Groups multiple queries that should be evaluated together, based on existence of common synonyms.
 */
void QueryGrouper::GroupClausesV2(std::vector<Clause*>* clauses, std::list<Group*>* groups,
                                    std::vector<Synonym>* target_synonyms,
                                    std::unordered_map<std::string, DesignEntity>* target_synonyms_map,
                                    std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_clause_indices) {
  // First create groups for clauses containing target synonyms (and their 'neighbouring' synonyms).
  std::unordered_set<int> visited_clauses;
  std::unordered_set<std::string> visited_target_synonyms;
  std::unordered_set<std::string> visited_synonyms;
  for (Synonym tgt_syn : *target_synonyms) {
    if (visited_target_synonyms.find(tgt_syn.GetName()) != visited_target_synonyms.end()) {
      continue;
    }
    // dfs starting from this target synonym
    QueryGrouper::DfsFromSynonym(target_synonyms_map, clauses, groups, tgt_syn.GetName(), &visited_clauses,
                                   &visited_target_synonyms, &visited_synonyms, map_of_syn_to_clause_indices);
  }
  // create groups for unvisited clauses
  for (int index_of_cl = 0; index_of_cl < clauses->size(); index_of_cl++) {
    if (visited_clauses.find(index_of_cl) != visited_clauses.end()) {
      continue;
    }
    std::vector<std::string> clause_syn_names = (*clauses)[index_of_cl]->GetAllSynonymNamesOfClause();
    // if cl has no syns -> add to its own group; and add to visited
    if (clause_syn_names.size() == 0) {
      Group* g = new Group();
      g->AddClauseToVector((*clauses)[index_of_cl]);
      groups->push_back(g);
      visited_clauses.insert(index_of_cl);
    } else {
      // same dfs routine with either arg treated as 'tgt syn'
      QueryGrouper::DfsFromSynonym(target_synonyms_map, clauses, groups, clause_syn_names[0], &visited_clauses,
                                     &visited_target_synonyms, &visited_synonyms, map_of_syn_to_clause_indices);
    }
  }
}

void QueryGrouper::DfsFromSynonym(std::unordered_map<std::string, DesignEntity>* tgt_synonyms_map,
                                    std::vector<Clause*>* clauses, std::list<Group*>* groups, std::string tgt_syn,
                                    std::unordered_set<int>* visited_clauses,
                                    std::unordered_set<std::string>* visited_tgt_synonyms,
                                    std::unordered_set<std::string>* visited_synonyms,
                                    std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_clause_indices) {
  Group* g = new Group();
  std::unordered_set<std::string> tgt_synonyms_in_this_group;
  bool is_starting_syn_a_target_syn = tgt_synonyms_map->find(tgt_syn) != tgt_synonyms_map->end();
  if (is_starting_syn_a_target_syn) {
    tgt_synonyms_in_this_group.insert(tgt_syn);
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
    for (auto cl_idx : cl_with_tgt_syns) {
      if (visited_clauses->find(cl_idx) != visited_clauses->end()) {
        continue;
      }
      subgroup.push_back(cl_idx);
    }
    // TODO: sort_with_comparator(subgroup) for optimization in iter 3
    for (auto cl_idx : subgroup) {
      visited_clauses->insert(cl_idx);
      std::vector<std::string> clause_syn_names = (*clauses)[cl_idx]->GetAllSynonymNamesOfClause();
      for (std::string clause_syn_name : clause_syn_names) {
        if (visited_synonyms->find(clause_syn_name) != visited_synonyms->end()) {
          continue;
        }
        if (tgt_synonyms_map->find(clause_syn_name) != tgt_synonyms_map->end()) {
          visited_tgt_synonyms->insert(clause_syn_name);
          tgt_synonyms_in_this_group.insert(clause_syn_name);
        }
        visited_synonyms->insert(clause_syn_name);
        q.push(clause_syn_name);
      }
      // push each clause in subgroup into group
      g->AddClauseToVector(clauses->at(cl_idx));
    }
  }
  if (g->GetClauses().size() > 0) {
    groups->push_back(g);
  }
  // populate group's metadata
  QueryGrouper::UpdateGroupMetadata(g, &tgt_synonyms_in_this_group, tgt_synonyms_map);
}

void QueryGrouper::UpdateGroupMetadata(Group* group, std::unordered_set<std::string>* tgt_synonyms_in_grp,
                                         std::unordered_map<std::string, DesignEntity>* tgt_syns_map) {
  for (auto item : *tgt_synonyms_in_grp) {
    group->AddSynToTargetSyns(Synonym(item, tgt_syns_map->at(item)));
  }
  group->UpdateHasTargetSynonymAttr();
}

/**
 * Groups up to 2 queries that should be evaluated together, based on existence of common synonyms.
 */
void QueryGrouper::GroupClauses(std::vector<Clause*>* clauses, std::list<Group*>* groups,
                                  std::vector<Synonym>* target_synonyms) {
  // [TODO iter 2]: implement grouping algorithm.
  // current implementation assumes only 1 such that and 1 pattern clause; manually grouping by common synonym.
  if (clauses->size() == 0) {
    return;
  }
  auto target = target_synonyms->at(0);
  if (clauses->size() == 1) {
    // expecting 1 such that or 1 pattern clause
    bool has_target_syn = false;
    // such that clause
    if (typeid(* (* clauses)[0]) == typeid(SuchThat)) {
      SuchThat* st = dynamic_cast<SuchThat*>((* clauses)[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
    } else {
      // pattern clause
      Pattern* pt = dynamic_cast<Pattern*>((* clauses)[0]);
      // has_target_syn if lhs is a target synonym, or if  syn-assn is a target synonym
      if (pt->assign_synonym.compare(target.GetName()) == 0 ||
          pt->left_is_synonym && pt->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
    }
    // add clause into its own group
    std::vector<Clause*> cl1;
    cl1.push_back((* clauses)[0]);
    Group* g1 = new Group(cl1, has_target_syn);
    groups->push_back(g1);

  } else if (clauses->size() == 2) {
    // expecting 1 such that followed by 1 pattern clause
    bool has_target_syn = false;
    if (typeid(* (* clauses)[0]) == typeid(SuchThat)) {
      SuchThat* st = dynamic_cast<SuchThat*>((* clauses)[0]);
      if (st->left_is_synonym && st->left_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      } else if (st->right_is_synonym && st->right_hand_side.compare(target.GetName()) == 0) {
        has_target_syn = true;
      }
    }
    std::vector<Clause*> cl1;
    cl1.push_back((* clauses)[0]);
    Group* g1 = new Group(cl1, has_target_syn);
    groups->push_back(g1);

    // if 1x pattern cl has common synonym, it goes in the same group.
    Pattern* pt = dynamic_cast<Pattern*>((* clauses)[1]);
    bool has_common_assign_syn = pt->assign_synonym.compare(cl1[0]->left_hand_side) == 0;
    bool has_common_args = pt->left_is_synonym && (
        pt->left_hand_side.compare(cl1[0]->left_hand_side) == 0 ||
            pt->left_hand_side.compare(cl1[0]->right_hand_side) == 0);
    if (has_common_assign_syn || has_common_args) {
      g1->AddClauseToVector(pt);
    } else {
      std::vector<Clause*> cl2;
      cl2.push_back(pt);
      bool pattern_cl_has_tgt_syn = false;
      if (pt->assign_synonym.compare(target.GetName()) == 0 ||
          pt->left_is_synonym && pt->left_hand_side.compare(target.GetName()) == 0) {
        pattern_cl_has_tgt_syn = true;
      }
      Group* g2 = new Group(cl2, pattern_cl_has_tgt_syn);
      groups->push_back(g2);
    }
  } else {
    throw PQLValidationException("Got more than 2 clauses.");
  }
}
