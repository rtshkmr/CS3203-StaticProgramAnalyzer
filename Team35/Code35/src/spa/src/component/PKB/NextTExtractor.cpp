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
                                              std::vector<Procedure*> proc_list,
                                              std::vector<Statement*> stmt_list) {
  stmt_list_ = stmt_list;
  int target_num = stoi(target);
  for (Procedure* proc: proc_list) {
    Cluster* proc_cluster = const_cast<Cluster*>(proc->GetClusterRoot());
    if (proc_cluster->CheckIfStmtNumInRange(target_num)) {
      Cluster* t_cluster = GetTargetCluster(proc_cluster, target_num);
      return GetNextTFromCluster(t_cluster, target_num);
    }
  }
  return std::vector<Entity*>{};
}

/**
 * Traverses the cluster on surface level and returns the outermost cluster with the target number.
 */
Cluster* NextTExtractor::GetTargetCluster(Cluster* p_cluster, int target_num) {
  std::list<Cluster*> nested_clusters = p_cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    return p_cluster;
  }

  auto cluster_iter = nested_clusters.begin();
  while (!(*cluster_iter)->CheckIfStmtNumInRange(target_num)) {
    std::advance(cluster_iter, 1);
  }
  return *cluster_iter;
}

/**
 * Go to innermost target block and start traversal. If any while loop is met, process from while.
 */
std::vector<Entity*> NextTExtractor::GetNextTFromCluster(Cluster* cluster, int target_num) {
  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    GetNextTByTraversal(dynamic_cast<Block*>(cluster), target_num);
  } else {
    Cluster* cluster_head = nested_clusters.front();
    Block* head_block = dynamic_cast<Block*>(cluster_head);
    if (head_block->isWhile) {
      GetNextTFromWhile(cluster, target_num);
    } else {
      Cluster* t_cluster = GetTargetCluster(cluster, target_num);
      return GetNextTFromCluster(t_cluster, target_num);
    }
  }
  if (next_t_map_.count(stmt_list_[target_num-1]) == 0) {
    return std::vector<Entity*>{};
  }
  return ltov(*next_t_map_[stmt_list_[target_num-1]]);
}

/**
 * Start recursive traversal from the block after the while loop. Adds all downstream relationships to all stmts in the
 * while loop.
 *
 * @param w_cluster Cluster representing the while loop
 * @return List of Entities that are Next* of the top of w_cluster
 */
std::list<Statement*> NextTExtractor::GetNextTFromWhile(Cluster* w_cluster, int target_num) {
  Block* w_block = dynamic_cast<Block*>(w_cluster->GetNestedClusters().front());
  Block* bigger_block = new Block();
  for (Block* next_block: w_block->GetNextBlocks()) {
    if (next_block->GetStartEndRange().first > bigger_block->GetStartEndRange().first) {
      bigger_block = next_block;
    }
  }
  assert(bigger_block->GetStartEndRange().first != -1);
  std::list<Entity*> next_t = GetNextTByTraversal(bigger_block, target_num);

  std::pair<int, int> range = w_cluster->GetStartEndRange();
  for (int i = range.first - 1; i < range.second; ++i) {
    std::vector<Statement*> w_statements;
    std::copy(stmt_list_[range.first - 1], stmt_list_[range.second - 1], std::back_inserter(w_statements));
    w_statements.erase(w_statements.begin() + i);
    AddVectorOfNextTRelationship(stmt_list_[i], w_statements);
  }
  return *next_t_map_[stmt_list_[w_block->GetStartEndRange().first-1]];
}

/**
 * Start recursive traversal
 *
 * @return List of Entities that Next* the Statement at the top of this block, or the target Statement.
 */
std::list<Statement*> NextTExtractor::GetNextTByTraversal(Block* block, int target_num) {
  std::list<Statement*> next_t;
  for (Block* next_block: block->GetNextBlocks()) {
    std::list<Statement*> next_block_next_t = GetNextTByTraversal(next_block, target_num);
    next_t.insert(next_t.end(), next_block_next_t.begin(), next_block_next_t.end());
  }

  std::pair<int, int> range = block->GetStartEndRange();
  if (target_num < range.first) {
    target_num = range.first;
  }

  for (int i = range.second; i <= target_num; --i) {
    std::vector<Statement*> b_statements(stmt_list_[i - 1], stmt_list_[range.second - 1]);

    AddListOfNextTRelationship(stmt_list_[i-1], next_t);
    AddVectorOfNextTRelationship(stmt_list_[i-1], b_statements);
  }
  return *next_t_map_[stmt_list_[target_num-1]];
}

void NextTExtractor::AddNextTRelationship(Statement* s1, Statement* s2) {
  std::list<Statement*> s{s1};
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

void NextTExtractor::AddListOfNextTRelationship(Statement* s1, std::list<Statement*> s2) {
  assert(next_t_map_.count(s1) == 0);
  next_t_map_[s1]->insert(next_t_map_[s1]->begin(), s2.begin(), s2.end());
}

std::vector<Entity*> NextTExtractor::ltov(std::list<Statement*> l) {
  std::vector<Entity*> v;
  v.reserve(l.size());
  std::copy(std::begin(l), std::end(l), std::back_inserter(v));
  return v;
}
