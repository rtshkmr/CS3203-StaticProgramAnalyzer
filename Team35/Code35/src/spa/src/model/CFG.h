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
  std::list<Cluster*> nested_clusters_;
 protected:
  int start_ = -1;
  int end_ = -1;
  Cluster* parent_cluster_;
 public:
  Cluster() {};
  int size() const;
  void AddChildCluster(Cluster* new_nested_cluster);
  void UpdateParentClusterRange(Cluster* new_nested_cluster);
  void AddSiblingCluster(Cluster* new_sibling_cluster);
  void AddStmt(StatementNumber statement_number);
  void RemoveStmt(StatementNumber statement_number);
  bool CheckIfStatementInRange(StatementNumber sn) const;
  Cluster* GetParentCluster();
  std::list<Cluster*> GetNestedClusters();
  Cluster* GetNextSiblingCluster();
  void SetParentCluster(Cluster* parent_cluster);
  /// ISSUE 2: to use dynamic_cast, need a virtual method; suggesstion -> create a virtual destructor (good practice too)
  virtual ~Cluster();
};

class Block : public Cluster {
  /// EXTRA THINGS TO ADD SINCE FOR SET (since set is sorting pointers)
  struct BlockComparator {
    bool operator()(const Block* lhs, const Block* rhs) const {
      return lhs->start_ < rhs->start_;
    }
  };

 public:
  Block() {};
  ~Block();

  bool isWhile = false;
  std::set<Block*> next_blocks_ = {};

};

class ConditionalBlock : public Block {
 private:
  std::set<VariableName*> control_variables_;

 public:
  ~ConditionalBlock();
  ConditionalBlock() {};
};

class BodyBlock : public Block {

 public:
  ~BodyBlock();
  BodyBlock() {};
};

#endif //AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
