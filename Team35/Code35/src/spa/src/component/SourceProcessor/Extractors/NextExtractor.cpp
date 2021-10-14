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

  for (Procedure* proc: deliverable_->proc_list_) {
    Cluster* proc_cluster = const_cast<Cluster*>(proc->GetClusterRoot());
    std::pair<int, int> range = proc_cluster->GetStartEndRange();
    int num_of_stmts = range.second - range.first + 1;
    std::vector<int>* visited_blocks = new std::vector<int>(num_of_stmts, 0);
    ExtractBlock(const_cast<Block*>(proc->GetBlockRoot()), visited_blocks);
  }
}

/**
 * Extracts next relationships from within the block and outward bound next relationships.
 *
 * @param block
 * @param visited_blocks
 * @throws out_of_range exception when index of block is not in stmt_list
 */
void NextExtractor::ExtractBlock(Block* block, std::vector<int>* visited_blocks) {
  if (block == nullptr) return;
  std::pair<int, int> range = block->GetStartEndRange();
  int start = range.first;
  if ((*visited_blocks)[start - 1] == 1) return;

  std::vector<Statement*> stmt_list = deliverable_->stmt_list_;

  Statement* start_statement = stmt_list[start - 1];
  for (int i = start + 1; i < range.second; ++i) {
    Statement* next_statement = stmt_list[i - 1];
    deliverable_->AddNextRelationship(start_statement, next_statement);
  }

  (*visited_blocks)[start - 1] = 1;

  int end = range.second;
  Statement* end_statement = stmt_list[end - 1];

  for (Block* next_block: block->next_blocks_) {
    int next = next_block->GetStartEndRange().first;

    if (next == -1) {
      continue;
    }

    Statement* next_block_statement = stmt_list[next - 1];
    deliverable_->AddNextRelationship(end_statement, next_block_statement);
    ExtractBlock(next_block, visited_blocks);
  }
}
