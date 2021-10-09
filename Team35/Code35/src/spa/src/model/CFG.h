/**
 * This file contains the classes used / required for CFG.
 */
#ifndef AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
#define AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_

#include <vector>
#include <set>
#include <datatype/DataType.h>
#include "Entity.h"

class Block;

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
  /// ISSUE 2: to use dynamic_cast, need a virtual method; suggesstion -> create a virtual destructor (good practice too)
  virtual std::set<Block*> GetNextBlock() { //to be overwritten by child. -> bad imp; [for testing use]
    throw std::invalid_argument("Should not access this if you are not Block");
  }

  // todo: add more helper functions:
  bool IsVariableModified(VariableName target_variable);
};

class Block: public Cluster {
  /// EXTRA THINGS TO ADD SINCE FOR SET (since set is sorting pointers)
  struct BlockComparator {
    bool operator()(const Block* lhs, const Block* rhs) const  {
      return lhs->start_ < rhs->start_;
    }
  };

 public:
  bool isWhile = false;
  // std::set<Block*, Block::BlockComparator> next_block_ = {};
  std::set<Block*> next_block_ = {};
  std::set<Block*> GetNextBlock() {
    return next_block_;
  }

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
