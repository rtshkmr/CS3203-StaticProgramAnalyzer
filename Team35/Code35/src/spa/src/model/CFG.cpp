/**
 * This file contains the implementation required for CFG.
 */

#include "CFG.h"

void Block::AddStmt(StatementNumber sn) {
  if (start == end && start == -1) { //new block
    start = end = sn.GetNum();
  } else {
    //acts as a deque.
    if (sn.GetNum() == start - 1) {
      start -= 1;
    } else if (sn.GetNum() == end + 1) {
      end += 1;
    } else {
      throw std::invalid_argument("Expectation of continuous numbers");
    }
  }
}

void Block::RemoveStmt(StatementNumber sn) {
  if (start == end && end == sn.GetNum()) {
    start = end = -1;
  } else if (sn.GetNum() == end) {
    end--;
  } else if (sn.GetNum() == start) {
    start++;
  } else {
    throw std::invalid_argument("Expectation of continuous numbers");
  }
}

bool Block::find(StatementNumber sn) {
  if (start == -1 && end == -1) return false;
  int num = sn.GetNum();
  return start <= num && num <= end;
}

int Block::size() {
  if (start == -1 && end == -1) return 0;
  return end - start + 1;
}
