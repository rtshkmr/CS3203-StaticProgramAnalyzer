#include <cassert>
#include "NextTExtractor.h"

/**
 * @return size of next_t_map
 */
int NextTExtractor::GetNextTSize() {
  return next_t_map_.size();
}

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
  proc_list_ = proc_list;
  stmt_list_ = stmt_list;
  Init();

  int target_num = stoi(target);
  for (Procedure* proc: proc_list) {
    Cluster* proc_cluster = const_cast<Cluster*>(proc->GetClusterRoot());
    if (proc_cluster->CheckIfStmtNumInRange(target_num)) {
      Cluster* t_cluster = GetTargetCluster(proc_cluster, target_num);
      return ltov(GetNextTFromCluster(t_cluster, target_num));
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
 * Assumes that map entry is only updated once.
 */
std::list<Statement*> NextTExtractor::GetNextTFromCluster(Cluster* cluster, int target_num) {
  if (next_t_map_.count(stmt_list_[target_num - 1]) == 1) {
    return *next_t_map_.find(stmt_list_[target_num - 1])->second;
  }

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
  if (next_t_map_.count(stmt_list_[target_num - 1]) == 0) {
    return std::list<Statement*>{};
  } else {
    return *next_t_map_.find(stmt_list_[target_num - 1])->second;
  }
}

/**
 * Start recursive traversal from the block after the while loop. Adds all downstream relationships to all stmts in the
 * while loop.
 *
 * @param w_cluster Cluster representing the while loop
 * @return List of Statements that are Next* of the top of w_cluster
 */
std::list<Statement*> NextTExtractor::GetNextTFromWhile(Cluster* w_cluster, int target_num) {
  if (next_t_map_.count(stmt_list_[target_num - 1]) == 1) {
    return *next_t_map_.find(stmt_list_[target_num - 1])->second;
  }

  Block* w_block = dynamic_cast<Block*>(w_cluster->GetNestedClusters().front());
  Block* block_after_w = w_block;
  for (Block* next_block: w_block->GetNextBlocks()) {
    // not the immediate block after the w cond
    if (next_block->GetStartEndRange().first != w_block->GetStartEndRange().first + 1) {
      block_after_w = next_block;
    }
  }
  std::list<Statement*> next_t_after_w;
  if (block_after_w != w_block) {
     next_t_after_w = GetNextTByTraversal(block_after_w, target_num);
     next_t_after_w.push_back(stmt_list_[block_after_w->GetStartEndRange().first-1]);
  }

  std::pair<int, int> range = w_cluster->GetStartEndRange();
  int count = range.second - range.first + 1;
  std::list<Statement*> w_statements(count);
  std::copy(&stmt_list_[range.first - 1], &stmt_list_[range.second - 1]+1, w_statements.begin());
  w_statements.insert(w_statements.end(), next_t_after_w.begin(), next_t_after_w.end());
  for (int i = range.first - 1; i < range.second; ++i) {
    AddNextT(stmt_list_[i], w_statements);
  }

  if (next_t_map_.count(stmt_list_[w_block->GetStartEndRange().first - 1]) == 0) {
    return std::list<Statement*>{};
  } else {
    return *next_t_map_.find(stmt_list_[w_block->GetStartEndRange().first - 1])->second;
  }
}

/**
 * Start recursive traversal
 *
 * @return List of Statements that Next* the Statement at the top of this block, or the target Statement.
 */
std::list<Statement*> NextTExtractor::GetNextTByTraversal(Block* block, int target_num) {
  if (next_t_map_.count(stmt_list_[target_num - 1]) == 1) {
    return *next_t_map_.find(stmt_list_[target_num - 1])->second;
  }

  if (block->isWhile) {
    return GetNextTFromWhile(block->GetParentCluster(), target_num);
  }

  std::list<Statement*> next_t;
  for (Block* next_block: block->GetNextBlocks()) {
    std::list<Statement*> next_block_next_t = GetNextTByTraversal(next_block, target_num);
    next_t.insert(next_t.end(), next_block_next_t.begin(), next_block_next_t.end());
    // if the first next* in the list is not the next_block (while cases), add next* of next_block
    if (next_t.empty() || next_t.front()->GetStatementNumber()->GetNum() != next_block->GetStartEndRange().first) {
      next_t.push_back(stmt_list_[next_block->GetStartEndRange().first-1]);
    }
  }

  std::pair<int, int> range = block->GetStartEndRange();
  if (target_num < range.first) {
    target_num = range.first;
  }

  if (block->GetNextBlocks().size() == 2) { // if block because while was handled separately
    AddNextTForIf(stmt_list_[range.second - 1], next_t);
  } else {
    AddNextT(stmt_list_[range.second - 1], next_t);
  }
  for (int i = range.second-1; i >= target_num; --i) {
    next_t.push_back(stmt_list_[i]);
    AddNextT(stmt_list_[i - 1], next_t);
  }

  if (next_t_map_.count(stmt_list_[target_num - 1]) == 0) {
    return std::list<Statement*>{};
  } else {
    return *next_t_map_.find(stmt_list_[target_num - 1])->second;
  }
}

/**
 * Assumes that there are no duplicates. Appending lists take O(1).
 */
void NextTExtractor::AddNextT(Statement* s1, std::list<Statement*> s2) {
  assert(next_t_map_.count(s1) == 0);
  if (s2.empty()) return;
  auto* list = new std::list<Statement*>();
  list->insert(list->begin(), s2.begin(), s2.end());
  next_t_map_.insert({s1, list});
}

/**
 * Adds Next* while checking for duplicates. Uses an array for tracking duplicates, taking O(n) time.
 */
void NextTExtractor::AddNextTForIf(Statement* s1, std::list<Statement*> s2) {
  assert(next_t_map_.count(s1) == 0);
  if (s2.empty()) return;

  auto* list = new std::list<Statement*>();
  int s1_num = s1->GetStatementNumber()->GetNum();
  for (Statement* s: s2) {
    int s_num = s->GetStatementNumber()->GetNum();
    if (next_t_array_[s1_num-1][s_num-1] == 0) {
      next_t_array_[s1_num-1][s_num-1] = 1;
      list->push_back(s);
    }
  }
  next_t_map_.insert({s1, list});
}

std::vector<Entity*> NextTExtractor::ltov(std::list<Statement*> l) {
  std::vector<Entity*> v;
  v.reserve(l.size());
  std::copy(std::begin(l), std::end(l), std::back_inserter(v));
  return v;
}

std::vector<Entity*> NextTExtractor::GetPrevT(std::string target,
                                              std::vector<Procedure*> proc_list,
                                              std::vector<Statement*> stmt_list) {
  return std::vector<Entity*>();
}

void NextTExtractor::Delete() {

}

void NextTExtractor::Init() {
  if (initialized_) return;
  initialized_ = true;
  int total = 0;
  for (Procedure* proc: proc_list_) {
    Cluster* proc_cluster = const_cast<Cluster*>(proc->GetClusterRoot());
    int last_stmt = proc_cluster->GetStartEndRange().second;
    total = last_stmt > total ? last_stmt : total;
  }
  for (int i = 0; i < total; ++i) {
    std::vector v(total, 0);
    next_t_array_.push_back(v);
  }
}
