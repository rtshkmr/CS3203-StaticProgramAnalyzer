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
  int start_ = -1;
  int end_ = -1;
 public:
  bool isWhile = false;
  std::set<Block*> next_block_ = {};

  Block() {};

  void AddStmt(StatementNumber);
  void RemoveStmt(StatementNumber);
  bool find(StatementNumber);
  int size();
};

class ConditionalBlock : public Block {
 private:
  std::set<VariableName*> control_variables_;
  std::set<ConstantValue*> control_constants_;

 public:
  ConditionalBlock() {};
  void AddControlVariable(VariableName* control_variable);
};

class BodyBlock : public Block {
 private:
  std::set<VariableName*> modified_variables_;

 public:
  BodyBlock() {};
};

#endif //AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
