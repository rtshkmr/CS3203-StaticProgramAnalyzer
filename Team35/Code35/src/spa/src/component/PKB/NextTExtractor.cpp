#include <cassert>
#include "NextTExtractor.h"

/**
 * Extracts list of Next* of the target from the CFG. Caches Next* relationships for blocks traversed.
 *
 * @param target String of the statement number to look for.
 * @return List of Entities that are Next* of the target.
 *
 * pseudocode
 * get to the block of the target stmt
 * if any while loop, next* all statements within
 * else recurse into next blocks
 * return next* of recursed block
 * add list of next* to this block
 */
std::vector<Entity*> NextTExtractor::GetNextT(std::string target,
                                              std::vector<Procedure*>* proc_list,
                                              std::vector<Statement*>* stmt_list) {
  stmt_list_ = stmt_list;
  int target_num = stoi(target);
  for (Procedure* proc: *proc_list) {
    Cluster* proc_cluster = const_cast<Cluster*>(proc->GetClusterRoot());
    if (proc_cluster->CheckIfStmtNumInRange(target_num)) {
      return GetNextTFromCluster(proc_cluster, target_num);
    }
  }
  return std::vector<Entity*>{};
}

std::vector<Entity*> NextTExtractor::GetNextTFromCluster(Cluster* cluster, int target_num) {
  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    return GetNextTFromBlock(dynamic_cast<Block*>(cluster), target_num);
  }

  auto cluster_iter = nested_clusters.begin();
  while (!(*cluster_iter)->CheckIfStmtNumInRange(target_num)) {
    std::advance(cluster_iter, 1);
  }

  std::list<Cluster*> target_nested_clusters = (*cluster_iter)->GetNestedClusters();
  if (target_nested_clusters.empty()) {
    return GetNextTFromBlock(dynamic_cast<Block*>(*cluster_iter), target_num);
  } else {
    Cluster* target_cluster = target_nested_clusters.front();
    Block* target_cluster_head = dynamic_cast<Block*>(target_cluster);
    if (target_cluster_head->isWhile) {
      return GetNextTFromWhile(target_cluster);
    }
    return GetNextTFromCluster((*cluster_iter), target_num);
  }
}

std::vector<Entity*> NextTExtractor::GetNextTFromBlock(Block* block, int target_num) {
  std::pair<int, int> range = block->GetStartEndRange();
  int curr_num = target_num;
  for (int i = target_num + 1; i <= range.second; ++i) {
    next_t_map_.insert(stmt_list_[curr_num - 1], stmt_list_[i - 1]);
  }
}

void NextTExtractor::AddNextTRelationship(Statement* s1, Statement* s2) {
  if (next_t_map_.count(s1)) {
    std::list<Statement*>* nexts = next_t_map_.find(s1)->second;
    if (std::find(nexts->begin(), nexts->end(), s2) == nexts->end()) {
      // add s2 if it does not exist in nexts
      nexts->push_back(s2);
    }
  } else {
    auto* list = new std::list<Statement*>();
    list->push_back(s2);
    next_t_map_.insert(std::make_pair(s1, list));
  }

  if (previous_t_map_.count(s2)) {
    std::list<Statement*>* previous = previous_t_map_.find(s2)->second;
    if (std::find(previous->begin(), previous->end(), s1) == previous->end()) {
      // add s1 if it does not exist in previous
      previous->push_back(s1);
    }
  } else {
    auto* list = new std::list<Statement*>();
    list->push_back(s1);
    previous_t_map_.insert(std::make_pair(s2, list));
  }
}

void NextTExtractor::AddVectorOfNextTRelationship(Statement* s1, std::vector<Statement*> s2) {
  assert(next_t_map_.count(s1) == 0);
  next_t_map_[s1]->insert(next_t_map_[s1]->begin(), s2.begin(), s2.end());
}
