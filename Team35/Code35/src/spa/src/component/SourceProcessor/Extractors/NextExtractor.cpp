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
    int num_of_stmts = const_cast<Cluster*>(proc->GetClusterRoot())->GetEnd()
        - const_cast<Cluster*>(proc->GetClusterRoot())->GetStart();
    std::vector<int>* visited_blocks = new std::vector<int>(num_of_stmts, 0);
    ExtractBlock(const_cast<Block*>(proc->GetBlockRoot()), visited_blocks);
  }
}

void NextExtractor::ExtractBlock(Block* block, std::vector<int>* visited_blocks) {
  if (block == nullptr) return;
  int start = block->GetStart();
  if ((*visited_blocks)[start] == 1) return;

  std::unordered_map<int, Statement *> snh = deliverable_->stmt_num_hash_;

  assert(snh.find(start) != snh.end());
  Statement* start_statement = snh.find(start)->second;
  for (int i = start + 1; i < block->GetEnd(); ++i) {
    Statement* next_statement = snh.find(i)->second;
    deliverable_->AddNextRelationship(start_statement, next_statement);
  }

  (*visited_blocks)[start] = 1;

  int end = block->GetEnd();
  assert(snh.find(end) != snh.end());
  Statement* end_statement = snh.find(end)->second;

  for (Block* next_block: block->next_blocks_) {
    int next = next_block->GetStart();
    Statement* next_block_statement = snh.find(next)->second;
    deliverable_->AddNextRelationship(end_statement, next_block_statement);
    ExtractBlock(next_block, visited_blocks);
  }
}
