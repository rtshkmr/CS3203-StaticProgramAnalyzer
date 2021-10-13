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

 protected:
  int start_ = -1;
  int end_ = -1;
  Cluster* parent_cluster_;
 public:
  Cluster() {};
  int size() const;
  std::pair<int, int> GetStartEndRange();
  void AddChildCluster(Cluster* new_nested_cluster);
  void UpdateRange(Cluster* nested_cluster);
  void UpdateClusterRange();
  void AddSiblingCluster(Cluster* new_sibling_cluster);
  void AddStmt(StatementNumber statement_number);
  void RemoveStmt(StatementNumber statement_number);
  bool CheckIfStatementInRange(StatementNumber sn) const;
  Cluster* GetParentCluster();
  std::list<Cluster*> GetNestedClusters() const;
  Cluster* GetNextSiblingCluster();
  void SetParentCluster(Cluster* parent_cluster);
  virtual ~Cluster();
  int GetStart();
  int GetEnd();

  std::list<Cluster*> nested_clusters_;
};

class Block : public Cluster {
 public:
  Block() {};
  ~Block();
  static Block* GetNewExitBlock();
  static bool IsExitBlock(Block* block);

  bool isWhile = false;
  std::set<Block*> next_blocks_ = {};

};

class ConditionalBlock : public Block {
  // todo: as an optimisation strat, see what can be kept in condi block to and body block
  //       to act as a negative list instead

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
