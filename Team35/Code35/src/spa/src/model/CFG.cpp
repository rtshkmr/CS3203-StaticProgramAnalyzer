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
void Cluster::AddChildCluster(Cluster* new_nested_cluster) {
  this->nested_clusters_.push_back(new_nested_cluster);
  new_nested_cluster->SetParentCluster(this);
//  this->UpdateParentClusterRange(new_nested_cluster);

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
  // get the parent cluster if it exists
  Cluster* sibling = nullptr;
  Cluster* parent_cluster = this->GetParentCluster();
  if (parent_cluster != nullptr) { // i.e. not outmost cluster:
    std::list<Cluster*> siblings = parent_cluster->GetNestedClusters();
    std::list<Cluster*>::iterator itr = std::find(siblings.begin(), siblings.end(), this);
    if (itr != end(siblings)) { // i.e. this exists, i can find myself using my parent
      int next_sibling_idx = std::distance(siblings.begin(), itr) + 1; // todo: check if 0 or 1 idx
      if (next_sibling_idx >= siblings.size()) {
        // there is no sibling to myself, this is not an error.
        return nullptr;
      } else {
        std::advance(itr, 1);
        return * itr;
      }
    } else {
      // todo: throw exception for this, not possible to have asituation where I'm looking for my next sibling but I can't find myself in my parent's list
      return nullptr;
    }
  } else {
    // todo: not sure if i need to throw an exception here
    return nullptr;
  }
//  Cluster* next


  // if doesn't exist, it means we are in outermost cluster, todo: not sure if do nothing or throw exception

  return nullptr;
}
std::list<Cluster*> Cluster::GetNestedClusters() {
  return this->nested_clusters_;
}

void Cluster::AddSiblingCluster(Cluster* new_sibling_cluster) {
  Cluster* existing_parent_cluster = parent_cluster_;
  if (parent_cluster_ == nullptr) { // the outermost cluster can't have any siblings
    throw std::invalid_argument("The outermost cluster (representing a procedure) should never have any siblings");
  }
  parent_cluster_->AddChildCluster(new_sibling_cluster);
}

/**
 * When a new_nested_cluster is added to a parent cluster, the parent cluster's range of values for statements, [start_, end_]
 * needs to be updated with logic similar to AddSmt to reflect the range expanded by the child.
 * @param new_nested_cluster
 */
void Cluster::UpdateParentClusterRange(Cluster* new_nested_cluster) {
  int new_cluster_start = new_nested_cluster->start_;
  int new_cluster_end = new_nested_cluster->end_;
  assert(new_cluster_start <= new_cluster_end
             && new_cluster_start >= -1 && new_cluster_end >= -1);
  if(this->start_ == new_cluster_start && this->end_ == new_cluster_end) return;
  if (this->start_ == end_ && this->start_ == -1) { //new cluster, doesn't have any nested clusters yet
    this->start_ = new_cluster_start;
    this->end_ = new_cluster_end;
  } else { // there are nested clusters within, assume the start and end range already updated
    bool new_cluster_appears_before_this = new_cluster_end == this->start_ - 1;
    bool new_cluster_appears_after_this = new_cluster_start == this->end_ + 1;
    if(new_cluster_appears_before_this){
      this->start_ = new_cluster_start;
    } else if (new_cluster_appears_after_this) {
      this->end_ = new_cluster_end;
    } else {
      throw std::invalid_argument("[UpdateParentClusterRange] An input is only valid if statement numbers are continuous with existing ones");
    }
  }
}

// default destructors:
Cluster::~Cluster() = default;
Block::~Block() = default;
ConditionalBlock::~ConditionalBlock() = default;
BodyBlock::~BodyBlock() = default;
