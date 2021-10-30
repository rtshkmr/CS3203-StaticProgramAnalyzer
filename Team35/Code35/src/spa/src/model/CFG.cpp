/**
 * This file contains the implementation required for CFG.
 */

#include <cassert>
#include "CFG.h"

int Cluster::size() const {
  if (start_ == -1 && end_ == -1) return 0;
  return end_ - start_ + 1;
}

/**
 * Adds a new cluster to list of clusters within it.
 * @param new_nested_cluster
 */
void Cluster::AddChildClusterToBack(Cluster* new_nested_cluster) {
  this->nested_clusters_.push_back(new_nested_cluster);
  new_nested_cluster->SetParentCluster(this);
}

void Cluster::AddChildClusterToFront(Cluster* new_nested_cluster) {
  this->nested_clusters_.push_front(new_nested_cluster);
  new_nested_cluster->SetParentCluster(this);
}

void Cluster::AddStmt(StatementNumber statement_number) {
  if (start_ == end_ && start_ == -1) { //new block
    start_ = end_ = statement_number.GetNum();
  } else {
    //acts as a deque.
    if (statement_number.GetNum() == start_ - 1) {
      start_ -= 1;
    } else if (statement_number.GetNum() == end_ + 1) {
      end_ += 1;
    } else {
      throw std::invalid_argument("An input is only valid if statement numbers are continuous with existing ones");
    }
  }
}
void Cluster::RemoveStmt(StatementNumber statement_number) {
  if (start_ == end_ && end_ == statement_number.GetNum()) {
    start_ = end_ = -1;
  } else if (statement_number.GetNum() == end_) {
    end_--;
  } else if (statement_number.GetNum() == start_) {
    start_++;
  } else {
    throw std::invalid_argument("Expectation of continuous numbers");
  }
}

/**
 * Checks if a statement number is within the range of statements [start, end]
 * @param sn
 * @return true if statement number falls in the inclusive range of start_ and end_
 */
bool Cluster::CheckIfStatementInRange(StatementNumber sn) const {
  if (start_ == -1 && end_ == -1) return false;
  int num = sn.GetNum();
  return start_ <= num && num <= end_;
}

bool Cluster::CheckIfStmtNumInRange(int num) {
  return start_ <= num && num <= end_;
}

Cluster* Cluster::GetParentCluster() {
  return this->parent_cluster_;
}
void Cluster::SetParentCluster(Cluster* parent_cluster) {
  this->parent_cluster_ = parent_cluster;

}

/**
 * A cluster will be contained within another parent cluster (except if it's the outermost procedure cluster) as an element
 * of a list of nested_clusters. Hence, a sibling cluster refers to a cluster
 * adjacent to current cluster in the list.
 * @return
 */
Cluster* Cluster::GetNextSiblingCluster() {
  Cluster* parent_cluster = this->GetParentCluster();
  if (parent_cluster != nullptr) { // i.e. not outmost cluster:
    std::list<Cluster*> siblings = parent_cluster->GetNestedClusters();
    auto itr = std::find(siblings.begin(), siblings.end(), this);
    if (itr != end(siblings)) { // i.e. this exists, i can find myself using my parent
      int next_sibling_idx = std::distance(siblings.begin(), itr) + 1;
      if (next_sibling_idx >= siblings.size()) {
        return nullptr;
      } else {
        std::advance(itr, 1);
        return * itr;
      }
    } else {
      assert(false);
    }
  } else {
    return nullptr;
  }
}

/**
 * Finds the next sibling that is either an if-cluster or a while cluster.
 * @return nullptr if no such sibling exists
 */
Cluster* Cluster::FindNextSiblingCluster() {
  Cluster* next_sibling = this->GetNextSiblingCluster();
  while (next_sibling) {
    const ClusterTag next_sibling_tag = next_sibling->GetClusterTag();
    if (next_sibling_tag == ClusterTag::kWhileCluster || next_sibling_tag == ClusterTag::kIfCluster) {
      return next_sibling;
    } else {
      next_sibling = next_sibling->GetNextSiblingCluster();
    }
  }
  return nullptr;
}

/**
 * Finds the next sibling cluster that matches the container type indicated (i.e. either a
 * @param container_type
 * @return nullptr if no such cluster exists
 */
Cluster* Cluster::FindNextSiblingCluster(ClusterTag container_type) {
  assert(container_type == ClusterTag::kIfCluster || container_type == ClusterTag::kWhileCluster);
  Cluster* next_sibling_cluster = this->FindNextSiblingCluster();
  while (next_sibling_cluster) {
    if (next_sibling_cluster->GetClusterTag() == container_type) {
      return next_sibling_cluster;
    } else {
      next_sibling_cluster = next_sibling_cluster->GetNextSiblingCluster();
    }
  }
  return nullptr;
}

Cluster* Cluster::GetPrevSiblingCluster() {
  Cluster* parent_cluster = this->GetParentCluster();
  if (parent_cluster != nullptr) { // i.e. not outmost cluster:
    std::list<Cluster*> siblings = parent_cluster->GetNestedClusters();
    std::list<Cluster*>::iterator itr = std::find(siblings.begin(), siblings.end(), this);
    if (itr != end(siblings)) { // i.e. this exists, i can find myself using my parent
      int prev_sibling_idx = std::distance(siblings.begin(), itr) - 1;
      if (prev_sibling_idx < 0) {
        return nullptr; // no previous sibling exists:
      } else {
        return * (--itr);
      }
    } else {
      assert(false);
    }
  } else {
    return nullptr;
  }
  return nullptr;
}

std::list<Cluster*> Cluster::GetNestedClusters() const {
  return this->nested_clusters_;
}

void Cluster::AddSiblingCluster(Cluster* new_sibling_cluster) {
  if (parent_cluster_ == nullptr) { // the outermost cluster can't have any siblings
    throw std::invalid_argument("The outermost cluster (representing a procedure) should never have any siblings");
  }
  parent_cluster_->AddChildClusterToBack(new_sibling_cluster);
}

/**
 * When a nested_cluster is added to a parent cluster, the parent cluster's range of values for statements, [start_, end_]
 * needs to be updated with logic similar to AddSmt to reflect the range expanded by the child.
 * @param nested_cluster
 */
void Cluster::UpdateRange(Cluster* nested_cluster) {
  int new_cluster_start = nested_cluster->start_;
  int new_cluster_end = nested_cluster->end_;
  bool is_valid_new_cluster_range = new_cluster_start <= new_cluster_end
      && new_cluster_start >= -1 && new_cluster_end >= -1;
  assert(is_valid_new_cluster_range);
  bool this_cluster_range_is_unassigned = this->start_ == end_ && this->start_ == -1;
  if (this_cluster_range_is_unassigned) {
    bool this_has_nested_clusters = !this->nested_clusters_.empty();
    if (this_has_nested_clusters) {
      int start_of_first_nested_cluster = this->nested_clusters_.front()->start_;
      int end_of_last_nested_cluster = this->nested_clusters_.back()->end_;
      this->start_ = start_of_first_nested_cluster;
      this->end_ = end_of_last_nested_cluster;
    } else { // set the range to be same as the incoming:
      this->start_ = new_cluster_start;
      this->end_ = new_cluster_end;
    }
  } else { // there are nested clusters within, assume the start and end range already updated
    bool new_cluster_appears_before_this = new_cluster_end + 1 == this->start_;
    bool new_cluster_appears_after_this = new_cluster_start == this->end_ + 1;
    if (new_cluster_appears_before_this) {
      this->start_ = new_cluster_start;
    } else if (new_cluster_appears_after_this) {
      this->end_ = new_cluster_end;
    } else {
      throw std::invalid_argument(
          "[UpdateClusterRange] An input is only valid if statement numbers are continuous with existing ones");
    }
  }
}
std::pair<int, int> Cluster::GetStartEndRange() const {
  return std::pair<int, int>(this->start_, this->end_);
}

void Cluster::UpdateClusterRange() {
  if (nested_clusters_.empty()) {
    return;
  }
  for (auto nested_cluster: this->nested_clusters_) {
    nested_cluster->UpdateClusterRange();
    bool nested_cluster_range_already_considered =
        this->start_ <= nested_cluster->start_ && this->end_ >= nested_cluster->end_;
    if (!nested_cluster_range_already_considered) {
      this->UpdateRange(nested_cluster);
    }
  }
}
bool Cluster::CheckIfStatementsInRange(int first_stmt, int second_stmt) const {
  return first_stmt >= start_ && second_stmt <= end_;
}

ClusterTag Cluster::GetClusterTag() const {
  return this->cluster_tag_;
}
void Cluster::SetClusterTag(ClusterTag cluster_tag) {
  this->cluster_tag_ = cluster_tag;
}

/**
 * Traverses cluster from first to second statement number, using the given pkb to retrieve
 * data where necessary. Is recursive in nature, where we try to call on a small subproblem by
 * restricting the start statement number where possible. This public subroutine calls on other
 * @param pkb_rel_refs
 * @param scoped_cluster
 * @param target_range
 * @param pkb
 * @param lhs_var
 * @return
 */
bool Cluster::TraverseScopedCluster(PKBRelRefs rel_ref,
                                    Cluster* scoped_cluster,
                                    std::pair<int, int> target_range,
                                    PKB* pkb,
                                    const std::string& lhs_var) {
  switch (rel_ref) {
    case PKBRelRefs::kAffects: {
      return Cluster::TraverseScopedClusterForAffects(scoped_cluster, target_range, pkb, lhs_var);
    };
    default: {
      return false;
    };

  }
  return false;
}

/**
 * Traverses a scoped cluster moving from first_stmt towards second_stmt in the target range and checks if the lhs_var remains unmodified along the way
 * @param scoped_cluster
 * @param target_range
 * @param pkb
 * @param lhs_var
 * @return
 */
bool Cluster::TraverseScopedClusterForAffects(Cluster* scoped_cluster,
                                              std::pair<int, int> target_range,
                                              PKB* pkb,
                                              const std::string& lhs_var) {
  assert(scoped_cluster->CheckIfStatementsInRange(target_range.first, target_range.second));
  bool scoped_cluster_does_not_modify_var = true;
  // get all the children, for each child, explore depth if needed:
  std::list<Cluster*> children = scoped_cluster->nested_clusters_;
  for (auto child: children) {
    bool child_does_not_modify_var = true;
    bool child_contains_second_stmt = child->CheckIfStmtNumInRange(target_range.second);
    auto child_range = child->GetStartEndRange();
    ClusterTag tag = child->GetClusterTag();
    bool is_not_if_or_while_cluster_constituents = tag != ClusterTag::kIfCond
        && tag != ClusterTag::kIfBody && tag != ClusterTag::kElseBody && tag != ClusterTag::kWhileCond
        && tag != ClusterTag::kWhileBody;
    assert(is_not_if_or_while_cluster_constituents); // genuinely don't know if this is a valid assumption

    if (tag == ClusterTag::kIfCluster) {
      // need to consider different branches:
      // todo: add if cluster logic to traversal: need to be able to jump around
      std::list<Cluster*> cluster_constituents = child->GetNestedClusters();
      // conditional don't modify shit, ignore it
      Cluster* if_cond = cluster_constituents.front();
      assert(if_cond->GetClusterTag() == ClusterTag::kIfCond);
      // check the if body block || else body block for the same
    } else if (tag == ClusterTag::kWhileCluster && child_contains_second_stmt) {
      // todo: add while cluster traversal logic if the second statement is inside. note that it's possible to not enter the while statement at all
    } else { // it's a simple block, no alternative paths to consider:
      if (child_range.first + 1
          != child_range.second) { // iterate
        // start with offset of +1 to avoid counting the first statement
        int for_loop_end = std::min(child_range.second + 1, target_range.second);
        for (int line_num = child_range.first + 1; line_num < for_loop_end; line_num++) {
          child_does_not_modify_var = !pkb->HasRelationship(PKBRelRefs::kModifies, std::to_string(line_num), lhs_var);
          // QQ: based on previous discussion here: https://discord.com/channels/877051619970269184/880007746647388180/893129063391187005 ,
          //     it's correct to say that for call statements also, this above line will work and we don't need
          //     to go do path traversal for the function calls right
          if (!child_does_not_modify_var) break;
        }
      }
      if (!child_does_not_modify_var) {
        scoped_cluster_does_not_modify_var = false; // it does modify, fails early
        break; // breaks the outer for loop
      }
    }
    scoped_cluster_does_not_modify_var = child_does_not_modify_var;
    // if this child cluster contains my second statement, then at the end of this for-loop's body, I can end my search
    if (child_contains_second_stmt) break;// breaks the outer for loop if aldy reached out goal
  }
  return scoped_cluster_does_not_modify_var;
}

// default destructors:
Cluster::~Cluster() = default;
Block::~Block() = default;
ConditionalBlock::~ConditionalBlock() = default;
BodyBlock::~BodyBlock() = default;
/**
 * Returns unique exit blocks for every call.
 * @return
 */
Block* Block::GetNewExitBlock() {
  Block* exit_block = new Block();
  exit_block->start_ = -1;
  exit_block->end_ = -1;
  return exit_block;
}

bool Block::IsExitBlock(Block* block) {
  std::pair<int, int> range = block->GetStartEndRange();
  return std::get<0>(range) == -1 && std::get<1>(range) == -1;
}

void Block::PatchEmptyBlocks(Block* redundant, Block* to) {
  if (!Block::IsExitBlock(redundant)) {
    throw std::invalid_argument("Redundant block is non empty");
  }

  for (auto* block: redundant->prev_blocks_) {
    block->next_blocks_.erase(redundant);
    block->AddNextBlock(to);
  }
}

void Block::AddNextBlock(Block* next_block) {
  this->next_blocks_.insert(next_block);
  next_block->prev_blocks_.insert(this);
  assert(this->next_blocks_.size() <= 2);
}

std::set<Block*> Block::GetNextBlocks() const {
  return this->next_blocks_;
}

std::set<Block*> Block::GetPrevBlocks() const {
  return this->prev_blocks_;
}
