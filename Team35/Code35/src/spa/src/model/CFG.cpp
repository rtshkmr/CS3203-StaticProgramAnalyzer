/**
 * This file contains the implementation required for CFG.
 */

#include "CFG.h"

void Block::AddStmt(StatementNumber sn) {
  if (start_ == end_ && start_ == -1) { //new block
    start_ = end_ = sn.GetNum();
  } else {
    //acts as a deque.
    if (sn.GetNum() == start_ - 1) {
      start_ -= 1;
    } else if (sn.GetNum() == end_ + 1) {
      end_ += 1;
    } else {
      throw std::invalid_argument("Expectation of continuous numbers");
    }
  }
}

void Block::RemoveStmt(StatementNumber sn) {
  if (start_ == end_ && end_ == sn.GetNum()) {
    start_ = end_ = -1;
  } else if (sn.GetNum() == end_) {
    end_--;
  } else if (sn.GetNum() == start_) {
    start_++;
  } else {
    throw std::invalid_argument("Expectation of continuous numbers");
  }
}

bool Block::find(StatementNumber sn) {
  if (start_ == -1 && end_ == -1) return false;
  int num = sn.GetNum();
  return start_ <= num && num <= end_;
}

int Block::size() {
  if (start_ == -1 && end_ == -1) return 0;
  return end_ - start_ + 1;
}

void ConditionalBlock::AddControlVariable(VariableName* control_variable) {
  this->control_variables_.insert(control_variable);
}
