/**
 * This file contains the classes used / required for CFG.
 */
#ifndef AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
#define AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_

#include <vector>
#include <set>
#include <datatype/DataType.h>

class Block {
 public:
  std::set<StatementNumber> stmtNoList = {};
  int start;
  int end;
  std::set<Block*> next_block_ = {};

  Block() {};
};

#endif //AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
