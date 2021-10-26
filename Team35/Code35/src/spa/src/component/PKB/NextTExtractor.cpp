#include <cassert>
#include "NextTExtractor.h"

void NextTExtractor::Delete() {}

/**
 * Initializes next_t_array using a procedure list to get the total number of stmts in the program.
 */
void NextTExtractor::Init(const std::vector<Statement*> &stmt_list) {
  if (initialized_) return;
  initialized_ = true;
  stmt_list_ = stmt_list;

  int total = stmt_list.size();
  next_t_2d_array_ = std::vector<std::vector<int>>(total, std::vector<int>(total));
}

/**
 * @return size of next_t_map
 */
int NextTExtractor::GetNextTSize() {
  return next_t_map_.size();
}

int NextTExtractor::GetPrevTSize() {
  return prev_t_map_.size();
}

/**
 * Extracts list of Next* of the target from the CFG. Caches Next* relationships for blocks traversed.
 *
 * @param target String of the statement number to look for.
 * @param proc_list List of procedures to get the target from.
 * @return List of Entities that are Next* of the target.
 *
 * pseudocode
 * get to the block of the target stmt
 * if any while loop, next* all statements within
 * else recurse into next blocks
 * return next* of recursed block
 * add list of next* to this block
 */
std::vector<Entity*> NextTExtractor::GetNextT(int target,
                                              std::vector<Procedure*> proc_list,
                                              std::vector<Statement*> stmt_list) {
  bool is_out_of_bounds = target > stmt_list.size() || target <= 0;
  if (is_out_of_bounds) {
    return std::vector<Entity*>{};
  } else if (next_t_map_.count(stmt_list[target - 1]) == 1) {
    return ltov(*next_t_map_.find(stmt_list[target - 1])->second);
  }

  Init(stmt_list);
  Cluster* proc_cluster = GetProcCluster(proc_list, target);
  if (proc_cluster) {
    Cluster* t_cluster = GetTargetCluster(proc_cluster, target);
    return ltov(GetNextTFromCluster(t_cluster, target));
  }
  return std::vector<Entity*>{};
}

Cluster* NextTExtractor::GetProcCluster(std::vector<Procedure*> proc_list, int target) {
  for (Procedure* proc: proc_list) {  // todo: optimise finding procedure of target stmt
    Cluster* proc_cluster = const_cast<Cluster*>(proc->GetClusterRoot());
    if (proc_cluster->CheckIfStmtNumInRange(target)) {
      return proc_cluster;
    }
  }
  return nullptr;
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
  return GetValueFromMap(next_t_map_, target_num);
}

std::list<Statement*> NextTExtractor::GetValueFromMap(std::unordered_map<Statement*, std::list<Statement*>*> map,
                                                      int stmt_num) {
  if (map.count(stmt_list_[stmt_num - 1]) == 0) {
    return std::list<Statement*>{};
  } else {
    return *map.find(stmt_list_[stmt_num - 1])->second;
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
  Block* block_after_w = GetNextBlockAfterWhile(w_block);
  std::list<Statement*> next_t_after_w;
  if (block_after_w != w_block) {
    next_t_after_w = GetNextTByTraversal(block_after_w, target_num);
    next_t_after_w.push_back(stmt_list_[block_after_w->GetStartEndRange().first - 1]);
  }

  std::pair<int, int> range = w_cluster->GetStartEndRange();
  int count = range.second - range.first + 1;
  std::list<Statement*> w_statements(count);
  std::copy(&stmt_list_[range.first - 1], &stmt_list_[range.second - 1] + 1, w_statements.begin());
  w_statements.insert(w_statements.end(), next_t_after_w.begin(), next_t_after_w.end());
  for (int i = range.first - 1; i < range.second; ++i) {
    AddNextT(stmt_list_[i], w_statements);
  }
  return GetValueFromMap(next_t_map_, w_block->GetStartEndRange().first);
}

Block* NextTExtractor::GetNextBlockAfterWhile(Block* w_block) {
  Block* block_after_w = w_block;
  for (Block* next_block: w_block->GetNextBlocks()) {
    // not the immediate block after the w cond
    if (next_block->GetStartEndRange().first != w_block->GetStartEndRange().first + 1) {
      block_after_w = next_block;
    }
  }
  return block_after_w;
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
  std::pair<int, int> range = block->GetStartEndRange();
  target_num = target_num < range.first ? range.first : target_num;
  std::list<Statement*> next_t = RecurseNextBlocks(block, target_num);

  if (block->GetNextBlocks().size() == 2) { // if block because while was handled separately
    AddNextTWithDup(stmt_list_[range.second - 1], next_t);
  } else {
    AddNextT(stmt_list_[range.second - 1], next_t);
  }
  for (int i = range.second - 1; i >= target_num; --i) {
    next_t.push_back(stmt_list_[i]);
    AddNextT(stmt_list_[i - 1], next_t);
  }
  return GetValueFromMap(next_t_map_, target_num);
}

std::list<Statement*> NextTExtractor::RecurseNextBlocks(Block* block, int target_num) {
  std::list<Statement*> next_t;
  for (Block* next_block: block->GetNextBlocks()) {
    if (next_block->GetStartEndRange().first == -1) { // exit block
      assert(next_block->GetNextBlocks().empty());
      continue;
    }
    std::list<Statement*> next_block_next_t = GetNextTByTraversal(next_block, target_num);
    next_t.insert(next_t.end(), next_block_next_t.begin(), next_block_next_t.end());
    if (next_t.empty() || !next_block->isWhile) {
      next_t.push_back(stmt_list_[next_block->GetStartEndRange().first - 1]);
    }
  }
  return next_t;
}

/**
 * Assumes that there are no duplicates. Appending lists take O(1).
 * Assumes that next_t_map will only be updated once.
 */
void NextTExtractor::AddNextT(Statement* s1, std::list<Statement*> s2) {
  if (s2.empty() || next_t_map_.count(s1) == 1) return;
  auto* list = new std::list<Statement*>();
  list->insert(list->begin(), s2.begin(), s2.end());
  next_t_map_.insert({s1, list});
  next_t_lhs_stmts_.push_back(s1);
}

/**
 * Adds Next* while checking for duplicates. Uses an array for tracking duplicates, taking O(n) time.
 * Assumes that next_t_map will only be updated once.
 */
void NextTExtractor::AddNextTWithDup(Statement* s1, std::list<Statement*> s2) {
  if (s2.empty() || next_t_map_.count(s1) == 1) return;
  int s1_num = s1->GetStatementNumber()->GetNum();
  auto* list = MakeUniqueList(s1_num, s2);
  next_t_map_.insert({s1, list});
  next_t_lhs_stmts_.push_back(s1);
}

std::list<Statement*>* NextTExtractor::MakeUniqueList(int s1_num, std::list<Statement*> list) {
  auto new_list = new std::list<Statement*>();
  std::vector<int> duplicates(stmt_list_.size(), 0);
  for (Statement* s: list) {
    int s_num = s->GetStatementNumber()->GetNum();
    if (duplicates[s_num - 1] == 0) {
      duplicates[s_num - 1] = 1;
      next_t_2d_array_[s_num - 1][s1_num - 1] = 1;
      next_t_2d_array_[s1_num - 1][s_num - 1] = 1;
      new_list->push_back(s);
    }
  }
  return new_list;
}

std::vector<Entity*> NextTExtractor::ltov(std::list<Statement*> l) {
  std::vector<Entity*> v;
  v.reserve(l.size());
  std::copy(std::begin(l), std::end(l), std::back_inserter(v));
  return v;
}

/**
 * Gets all Entities that can be on the LHS of the relationship, i.e. Next*(s, _).
 * @param proc_list Full list of procedures.
 * @param stmt_list Full list of statements.
 * @return all Entities that can be on the LHS of the relationship.
 */
std::vector<Entity*> NextTExtractor::GetAllNextTLHS(std::vector<Procedure*> proc_list,
                                                    std::vector<Statement*> stmt_list) {
  if (next_t_populated_) {
    return next_t_lhs_stmts_;
  }
  Init(stmt_list);

  PopulateAllNextT(proc_list);
  return next_t_lhs_stmts_;
}

void NextTExtractor::PopulateAllNextT(std::vector<Procedure*> proc_list) {
  for (Procedure* proc: proc_list) {
    int first_stmt = const_cast<Cluster*>(proc->GetClusterRoot())->GetStartEndRange().first;
    GetNextT(first_stmt, {proc}, stmt_list_);
  }
  next_t_populated_ = true;
}

/**
 * Gets all Entity pairs that are in a Next* relationship, i.e. Next*(s1, s2).
 * @param proc_list Full list of procedures.
 * @param stmt_list Full list of statements.
 * @return all Entity pairs that are in a Next* relationship.
 */
std::vector<std::tuple<Entity*, Entity*>> NextTExtractor::GetAllNextT(std::vector<Procedure*> proc_list,
                                                                      std::vector<Statement*> stmt_list) {
  if (got_all_next_prev_t_) {
    return all_next_t_;
  }
  Init(stmt_list);

  if (!next_t_populated_) {
    PopulateAllNextT(proc_list);
  }
  for (auto pair: next_t_map_) {
    for (Statement* stmt: *pair.second) {
      all_next_t_.push_back({pair.first, stmt});
      all_prev_t_.push_back({stmt, pair.first});
    }
  }
  got_all_next_prev_t_ = true;
  return all_next_t_;
}

/**
 * Returns true if Next*(first, second).
 */
bool NextTExtractor::HasNextT(int first,
                              int second,
                              std::vector<Procedure*> proc_list,
                              std::vector<Statement*> stmt_list) {
  int total_stmt = stmt_list.size();
  if (first > total_stmt || first <= 0 || second > total_stmt || second <= 0) {
    return false;
  }
  Init(stmt_list);
  if (next_t_2d_array_[first - 1][second - 1] == 1) {
    return true;
  }

  Cluster* proc_cluster = GetProcCluster(proc_list, first);
  if (proc_cluster) {
    if (proc_cluster->CheckIfStmtNumInRange(second)) {
      Cluster* t_cluster = GetTargetCluster(proc_cluster, first);
      return HasNextTInFirstCluster(t_cluster, first, second);
    } else {  // first and second not in the same procedure
      return false;
    }
  }
  return false;
}

/**
 * Checks if the outermost cluster is while, and the first and second is either both in the while loop or
 * the second is after first. Else find HasNextTInCluster.
 *
 * @param cluster cluster that contains first.
 * @param first statement number.
 * @param second statement number.
 * @return true if Next*(first, second).
 */
bool NextTExtractor::HasNextTInFirstCluster(Cluster* cluster, int first, int second) {
  assert(cluster->CheckIfStmtNumInRange(first));
  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    return IsNextTDownstream(first, second);
  } else {
    Block* first_block = dynamic_cast<Block*>(nested_clusters.front());
    if (first_block->isWhile) {
      if (first <= second || cluster->CheckIfStmtNumInRange(second)) {
        next_t_2d_array_[first - 1][second - 1] = 1;
        return true;
      } else {  // second is before first and not in the same while loop of the outermost cluster
        return false;
      }
    } else {  // first cluster not a while loop
      Cluster* inner_cluster = GetTargetCluster(cluster, first);
      return HasNextTInCluster(inner_cluster, first, second);
    }
  }
}

/**
 * Checks if second statement number is downstream of first statement number. Assumes that both are in the same path.
 */
bool NextTExtractor::IsNextTDownstream(int first, int second) {
  if (first < second) {
    next_t_2d_array_[first - 1][second - 1] = 1;
    return true;
  } else {
    return false;
  }
}

/**
  * Gets to the innermost cluster containing first to find if Next*(first, second).
  * cluster must contain first. first and second must be in the same procedure.
  * Must traverse to conclusively determine Next* as this cluster is nested.
  * Pseudocode:
  * first >= second:
  * check for while loop then traverse
  * first < second:
  * traverse through clusters
  */
bool NextTExtractor::HasNextTInCluster(Cluster* cluster, int first, int second) {
  assert(cluster->CheckIfStmtNumInRange(first));
  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    return HasNextTByTraversal(dynamic_cast<Block*>(cluster), first, second);
  } else {
    Block* first_block = dynamic_cast<Block*>(nested_clusters.front());
    if (first_block->isWhile) {
      if (cluster->CheckIfStmtNumInRange(second)) {
        next_t_2d_array_[first - 1][second - 1] = 1;
        return true;
      } else if (second < first) {
        return false;
      } else {  // second > first
        Block* block_after_w = GetNextBlockAfterWhile(first_block);
        return HasNextTByTraversal(block_after_w, first, second);
      }
    } else {
      Cluster* inner_cluster = GetTargetCluster(cluster, first);
      return HasNextTInCluster(inner_cluster, first, second);
    }
  }
}

/**
 * Traverse and check. first should be guaranteed to be in this cluster or a previous cluster.
 * Cluster traversal cannot work on a nested level because of else block being the next sibling of if body block.
 */
bool NextTExtractor::HasNextTByTraversal(Block* block, int first, int second) {
  if (block->CheckIfStmtNumInRange(second)) {
    return IsNextTDownstream(first, second);
  } else {
    bool result = false;
    if (!block->CheckIfStmtNumInRange(first)) {
      next_t_2d_array_[first - 1][block->GetStartEndRange().first - 1] = 1;
    }
    for (Block* next_block: block->GetNextBlocks()) {
      if (next_t_2d_array_[first - 1][next_block->GetStartEndRange().first - 1] != 1) {
        result = result || HasNextTByTraversal(next_block, first, second);
      }
    }
    return result;
  }
}

/**
 * Extracts list of Next* that has target on the rhs, by dfs up the cfg.
 * @param target Statement number.
 * @param proc_list Full list of procedures.
 * @param stmt_list Full list of statements.
 * @return list of Next* that has target on the rhs.
 */
std::vector<Entity*> NextTExtractor::GetPrevT(int target,
                                              std::vector<Procedure*> proc_list,
                                              std::vector<Statement*> stmt_list) {
  bool is_out_of_bounds = target > stmt_list.size() || target <= 0;
  if (is_out_of_bounds) {
    return std::vector<Entity*>{};
  } else if (prev_t_map_.count(stmt_list[target - 1]) == 1) {
    return ltov(*prev_t_map_.find(stmt_list[target - 1])->second);
  }

  Init(stmt_list);
  Cluster* proc_cluster = GetProcCluster(proc_list, target);
  if (proc_cluster) {
    Cluster* t_cluster = GetTargetCluster(proc_cluster, target);
    return ltov(GetPrevTFromCluster(t_cluster, target));
  }
  return std::vector<Entity*>{};
}

/**
 * Go to innermost target block and start traversal. If any while loop is met, process from while.
 * Assumes that map entry is only updated once.
 */
std::list<Statement*> NextTExtractor::GetPrevTFromCluster(Cluster* cluster, int target) {
  if (prev_t_map_.count(stmt_list_[target - 1]) == 1) {
    return *prev_t_map_.find(stmt_list_[target - 1])->second;
  }

  std::list<Cluster*> nested_clusters = cluster->GetNestedClusters();
  if (nested_clusters.empty()) {
    GetPrevTByTraversal(dynamic_cast<Block*>(cluster), target);
  } else {
    Cluster* cluster_head = nested_clusters.front();
    Block* head_block = dynamic_cast<Block*>(cluster_head);
    if (head_block->isWhile) {
      GetPrevTFromWhile(cluster, target);
    } else {
      Cluster* t_cluster = GetTargetCluster(cluster, target);
      return GetPrevTFromCluster(t_cluster, target);
    }
  }
  return GetValueFromMap(prev_t_map_, target);
}

/**
 * Start recursive traversal from the block before the while loop. Adds all upstream relationships to all stmts in the
 * while loop.
 *
 * @param w_cluster Cluster representing the while loop
 * @return List of Statements that are prev* of the bottom of w_cluster
 */
std::list<Statement*> NextTExtractor::GetPrevTFromWhile(Cluster* w_cluster, int target_num) {
  if (prev_t_map_.count(stmt_list_[target_num - 1]) == 1) {
    return *prev_t_map_.find(stmt_list_[target_num - 1])->second;
  }

  Block* w_block = dynamic_cast<Block*>(w_cluster->GetNestedClusters().front());
  std::list<Block*> prev_blocks = GetPrevBlocksBeforeWhile(w_block);
  std::list<Statement*> prev_t_before_w;
  for (Block* prev_block: prev_blocks) {
    std::list<Statement*> prev_stmts = GetPrevTByTraversal(prev_block, target_num);
    prev_t_before_w.insert(prev_t_before_w.end(), prev_stmts.begin(), prev_stmts.end());
    prev_t_before_w.push_back(stmt_list_[prev_block->GetStartEndRange().second - 1]);
  }

  std::pair<int, int> range = w_cluster->GetStartEndRange();
  prev_t_before_w = *MakeUniqueList(range.first, prev_t_before_w);
  int count = range.second - range.first + 1;
  std::list<Statement*> w_statements(count);
  std::copy(&stmt_list_[range.first - 1], &stmt_list_[range.second - 1] + 1, w_statements.begin());
  w_statements.insert(w_statements.end(), prev_t_before_w.begin(), prev_t_before_w.end());
  for (int i = range.first - 1; i < range.second; ++i) {
    AddPrevT(stmt_list_[i], w_statements);
  }
  return GetValueFromMap(prev_t_map_, w_cluster->GetStartEndRange().second);
}

std::list<Block*> NextTExtractor::GetPrevBlocksBeforeWhile(Block* w_block) {
  std::list<Block*> prev_blocks;
  for (Block* prev_block: w_block->GetPrevBlocks()) {
    if (prev_block->GetStartEndRange().first < w_block->GetStartEndRange().first) {
      prev_blocks.push_back(prev_block);
    }
  }
  return prev_blocks;
}

/**
 * Start recursive traversal
 *
 * @return List of Statements that Prev* the Statement at the bottom of this block, or the target Statement.
 */
std::list<Statement*> NextTExtractor::GetPrevTByTraversal(Block* block, int target_num) {
  if (prev_t_map_.count(stmt_list_[target_num - 1]) == 1) {
    return *prev_t_map_.find(stmt_list_[target_num - 1])->second;
  }
  if (block->isWhile) {
    return GetPrevTFromWhile(block->GetParentCluster(), target_num);
  }
  std::pair<int, int> range = block->GetStartEndRange();
  target_num = target_num > range.second ? range.second : target_num;
  std::list<Statement*> prev_t = RecursePrevBlocks(block, target_num);

  if (block->GetPrevBlocks().size() >= 2) { // if block because while was handled separately
    AddPrevTWithDup(stmt_list_[range.first - 1], prev_t);
  } else {
    AddPrevT(stmt_list_[range.first - 1], prev_t);
  }
  for (int i = range.first - 1; i < target_num - 1; ++i) {
    prev_t.push_back(stmt_list_[i]);
    AddPrevT(stmt_list_[i + 1], prev_t);
  }
  return GetValueFromMap(prev_t_map_, target_num);
}

std::list<Statement*> NextTExtractor::RecursePrevBlocks(Block* block, int target_num) {
  std::list<Statement*> prev_t;
  for (Block* prev_block: block->GetPrevBlocks()) {
    std::list<Statement*> prev_block_prev_t = GetPrevTByTraversal(prev_block, target_num);
    prev_t.insert(prev_t.end(), prev_block_prev_t.begin(), prev_block_prev_t.end());
    if (prev_t.empty() || !prev_block->isWhile) {
      prev_t.push_back(stmt_list_[prev_block->GetStartEndRange().second - 1]);
    }
  }
  return prev_t;
}

/**
 * Assumes that there are no duplicates. Appending lists take O(1).
 * Assumes that prev_t_map will only be updated once.
 */
void NextTExtractor::AddPrevT(Statement* s1, std::list<Statement*> s2) {
  if (s2.empty() || prev_t_map_.count(s1) == 1) return;
  auto* list = new std::list<Statement*>();
  list->insert(list->begin(), s2.begin(), s2.end());
  prev_t_map_.insert({s1, list});
  next_t_rhs_stmts_.push_back(s1);
}

/**
 * Adds Prev* while checking for duplicates. Uses an array for tracking duplicates, taking O(n) time.
 * Assumes that prev_t_map will only be updated once.
 */
void NextTExtractor::AddPrevTWithDup(Statement* s1, std::list<Statement*> s2) {
  if (s2.empty() || prev_t_map_.count(s1) == 1) return;
  int s1_num = s1->GetStatementNumber()->GetNum();
  auto* list = MakeUniqueList(s1_num, s2);
  prev_t_map_.insert({s1, list});
  next_t_rhs_stmts_.push_back(s1);
}

/**
 * Gets all Entities that can be on the RHS of the relationship, i.e. Next*(_, s).
 * @param proc_list Full list of procedures.
 * @param stmt_list Full list of statements.
 * @return all Entities that can be on the RHS of the relationship.
 */
std::vector<Entity*> NextTExtractor::GetAllNextTRHS(std::vector<Procedure*> proc_list,
                                                    std::vector<Statement*> stmt_list) {
  if (prev_t_populated_) {
    return next_t_rhs_stmts_;
  }
  Init(stmt_list);

  PopulateAllPrevT(proc_list);
  return next_t_rhs_stmts_;
}

void NextTExtractor::PopulateAllPrevT(std::vector<Procedure*> proc_list) {
  for (Procedure* proc: proc_list) {
    int last_stmt = const_cast<Cluster*>(proc->GetClusterRoot())->GetStartEndRange().second;
    GetPrevT(last_stmt, {proc}, stmt_list_);
  }
  prev_t_populated_ = true;
}

/**
 * Gets all Entity pairs that are in a Next* relationship, i.e. Next*(s1, s2).
 * @param proc_list Full list of procedures.
 * @param stmt_list Full list of statements.
 * @return all Entity pairs in reverse order i.e. <s2, s1> of Next*(s1, s2).
 */
std::vector<std::tuple<Entity*, Entity*>> NextTExtractor::GetAllPrevT(std::vector<Procedure*> proc_list,
                                                                      std::vector<Statement*> stmt_list) {
  if (got_all_next_prev_t_) {
    return all_prev_t_;
  }
  Init(stmt_list);

  if (!prev_t_populated_) {
    PopulateAllPrevT(proc_list);
  }
  for (auto pair: prev_t_map_) {
    for (Statement* stmt: *pair.second) {
      all_prev_t_.push_back({pair.first, stmt});
      all_next_t_.push_back({stmt, pair.first});
    }
  }
  got_all_next_prev_t_ = true;
  return all_prev_t_;
}
