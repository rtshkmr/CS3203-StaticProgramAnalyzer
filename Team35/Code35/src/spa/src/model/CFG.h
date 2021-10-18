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

 protected:
  int start_ = -1;
  int end_ = -1;
  Cluster* parent_cluster_;
 public:
  Cluster() {};
  int size() const;
  std::pair<int, int> GetStartEndRange();
  void AddChildClusterToBack(Cluster* new_nested_cluster);
  void AddChildClusterToFront(Cluster* new_nested_cluster);
  void UpdateRange(Cluster* nested_cluster);
  void UpdateClusterRange();
  void AddSiblingCluster(Cluster* new_sibling_cluster);
  void AddStmt(StatementNumber statement_number);
  void RemoveStmt(StatementNumber statement_number);
  bool CheckIfStatementInRange(StatementNumber sn) const;
  Cluster* GetParentCluster();
  std::list<Cluster*> GetNestedClusters() const;
  Cluster* GetNextSiblingCluster();
  Cluster* GetPrevSiblingCluster();
  void SetParentCluster(Cluster* parent_cluster);
  virtual ~Cluster();

  // only for testing
  void SetStartEnd(int start, int end) {
    start_ = start;
    end_ = end;
  };
  std::list<Cluster*> nested_clusters_;
};

class Block : public Cluster {
 private:
  std::set<Block*> next_blocks_ = {};
  std::set<Block*> prev_blocks_ = {};

 public:
  Block() {};
  ~Block();
  static Block* GetNewExitBlock();
  static bool IsExitBlock(Block* block);
  static void PatchEmptyBlocks(Block* redundant, Block* to);
  void AddNextBlock(Block* next_block);
  std::set<Block*> GetNextBlocks() const;

  bool isWhile = false;
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