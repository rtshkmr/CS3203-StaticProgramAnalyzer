/**
 * This file contains the classes used / required for CFG.
 */
#ifndef AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
#define AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_

#include <vector>
#include <set>
#include <datatype/DataType.h>
#include "Entity.h"

class Cluster {

 private:
  std::list<Cluster> clusters_;
 protected:
  int start_ = -1;
  int end_ = -1;
  std::set<VariableName*> modified_variables_; // QQ: why does it throw error if I use Variable instead of VariableName* !?
 public:
  Cluster() {};
  int size() const;
  void AddCluster(Cluster new_cluster);
  void AddStmt(StatementNumber statement_number);
  void RemoveStmt(StatementNumber statement_number);
  bool CheckIfStatementInCluster(StatementNumber sn) const;

  // todo: add more helper functions:
  bool IsVariableModified(VariableName target_variable);
};



class Block: public Cluster {
 public:
  bool isWhile = false;
  std::set<Block*> next_block_ = {};

  Block(){};
};

class ConditionalBlock : public Block {
 private:
  std::set<VariableName*> control_variables_;
  std::set<ConstantValue*> control_constants_;

 public:
  ConditionalBlock(){};
  void AddControlVariable(VariableName* control_variable);
};

class BodyBlock : public Block {
 private:
  std::set<VariableName*> modified_variables_;

 public:
  BodyBlock(){};
};

#endif //AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
