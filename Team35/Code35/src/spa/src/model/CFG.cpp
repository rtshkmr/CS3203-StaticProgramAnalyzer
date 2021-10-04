/**
 * This file contains the implementation required for CFG.
 */

#include "CFG.h"

void Block::AddStmt(StatementNumber sn) {
  if (stmtNoList.size() == 0)
    start = sn.GetNum();
  this->stmtNoList.insert(sn);
  end = sn.GetNum(); //assume continuous
}

void Block::RemoveStmt(StatementNumber sn) {
  stmtNoList.erase(sn);

  if (stmtNoList.size() == 0) {
    start = end = 0;
  } else if (sn.GetNum() == end) {
    end--; //assume continuous
  } else if (sn.GetNum() == start) {
    start++; //assume continuous
  }
  return;
}

int Block::StmtListSize() {
  return stmtNoList.size();
}
