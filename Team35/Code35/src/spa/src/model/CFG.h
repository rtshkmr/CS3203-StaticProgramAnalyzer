/**
 * This file contains the classes used / required for CFG.
 */
#ifndef AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
#define AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_

#include <vector>
#include <set>
#include <datatype/DataType.h>

class Block {
 private:
  int start = -1;
  int end = -1;
 public:
  bool isWhile = false;
  std::set<Block*> next_block_ = {};

  Block() {};

  void AddStmt(StatementNumber);
  void RemoveStmt(StatementNumber);
  bool find(StatementNumber);
  int size();
};

#endif //AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
