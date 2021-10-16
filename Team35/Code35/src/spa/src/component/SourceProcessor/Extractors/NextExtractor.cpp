#include <cassert>
#include "NextExtractor.h"

/**
 * Extracts Next relationships into the deliverable using the CFG generated.
 *
 * pseudocode:
 * for every procedure in proc list
 * thread block
 * return if the block nullptr or visited
 * mark visited
 * get next
 * for each next
 *  add next rln to this block
 *  thread next block
 */
void NextExtractor::Extract(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

  int total_num_of_stmts = 0;
  for (Procedure* proc: deliverable_->proc_list_) {
    Cluster* proc_cluster = const_cast<Cluster*>(proc->GetClusterRoot());
    std::pair<int, int> range = proc_cluster->GetStartEndRange();
    int num_of_stmts = range.second - range.first + 1;
    total_num_of_stmts += num_of_stmts;
  }
  for (Procedure* proc: deliverable_->proc_list_) {
    visited_blocks_ = new std::vector<int>(total_num_of_stmts, 0);
    ExtractBlock(const_cast<Block*>(proc->GetBlockRoot()));
  }
}

/**
 * Extracts next relationships from within the block and outward bound next relationships.
 *
 * @param block
 * @param visited_blocks_
 * @throws out_of_range exception when index of block is not in stmt_list
 */
void NextExtractor::ExtractBlock(Block* block) {
  if (block == nullptr) return;
  std::pair<int, int> range = block->GetStartEndRange();
  int start = range.first;
  if ((*visited_blocks_)[start - 1] == 1) return;

  std::vector<Statement*> stmt_list = deliverable_->stmt_list_;

  Statement* prev_statement = stmt_list[start - 1];
  for (int i = start + 1; i <= range.second; ++i) {
    Statement* next_statement = stmt_list[i - 1];
    deliverable_->AddNextRelationship(prev_statement, next_statement);
    prev_statement = next_statement;
  }

  (*visited_blocks_)[start - 1] = 1;

  int end = range.second;
  Statement* end_statement = stmt_list[end - 1];

  for (Block* next_block: block->GetNextBlocks()) {
    int next = next_block->GetStartEndRange().first;

    assert(next != -1); // uninitialized block

    Statement* next_block_statement = stmt_list[next - 1];
    deliverable_->AddNextRelationship(end_statement, next_block_statement);
    ExtractBlock(next_block);
  }
}
