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
    // todo: change to visited_array "bitmap" for each cfg
    ExtractBlock(const_cast<Block*>(proc->GetBlockRoot()));
  }
}

void NextExtractor::ExtractBlock(Block* block) {
  if (block == nullptr) return;
  auto iter = std::find(visited_blocks.begin(), visited_blocks.end(), block);
  if (iter == visited_blocks.end()) return;

  int previous = block->GetStart();
  for (int i = previous + 1; i < block->GetEnd(); ++i) {
    deliverable_->AddNextRelationship(previous, i); // todo: stmt num to stmt map
  }

  visited_blocks.push_back(block);

  for (Block* next_block: block->next_blocks_) {
    deliverable_->AddNextRelationship(block, next_block);
    ExtractBlock(next_block);
  }
}
