/**
 * This file contains the classes used / required for CFG.
 */
#ifndef AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
#define AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_

#include <vector>
#include <set>
#include <datatype/DataType.h>
#include <component/QueryProcessor/types/Types.h>
#include <component/PKB/PKB.h>
#include "Entity.h"

/**
 * Tags cluster objects with the kind of code that they contain.
 */
enum class ClusterTag {
  kIfCluster,
  kWhileCluster,
  kProcedureCluster,
  kWhileCondBlock,
  kIfCondBlock,
  kWhileBody,
  kIfBody,
  kElseBody,
  kNormalBlock // default tags since clusters will be tagged
};

class Cluster {

 protected:
  ClusterTag cluster_tag_ = ClusterTag::kNormalBlock;
  int start_ = -1;
  int end_ = -1;
  Cluster* parent_cluster_;

 public:
  Cluster() = default;;
  int size() const;
  std::pair<int, int> GetStartEndRange() const;
  ClusterTag GetClusterTag() const;
  void SetClusterTag(ClusterTag cluster_tag);
  void AddChildClusterToBack(Cluster* new_nested_cluster);
  void AddChildClusterToFront(Cluster* new_nested_cluster);
  void UpdateRange(Cluster* nested_cluster);
  void UpdateClusterRange();
  void AddSiblingCluster(Cluster* new_sibling_cluster);
  void AddStmt(StatementNumber statement_number);
  void RemoveStmt(StatementNumber statement_number);
  bool CheckIfStatementInRange(StatementNumber sn) const;
  bool CheckIfStmtNumInRange(int num);
  bool CheckIfStatementsInRange(int first_stmt, int second_stmt) const;
  Cluster* GetParentCluster();
  std::list<Cluster*> GetNestedClusters() const;
  Cluster* GetNextSiblingCluster();
  Cluster* GetPrevSiblingCluster();
  void SetParentCluster(Cluster* parent_cluster);
  virtual ~Cluster();

  std::list<Cluster*> nested_clusters_;
  Cluster* GetClusterConstituent(ClusterTag constituent_tag);
  Cluster* FindNextSibling(ClusterTag target_tag);
  static bool TraverseScopedCluster(PKBRelRefs pkb_rel_refs, Cluster* scoped_cluster,
                                    std::pair<int, int> target_range,
                                    PKB* pkb,
                                    const std::string& lhs_var);
  static bool CheckScopeClusterForAffects(Cluster* scoped_cluster,
                                              std::pair<int, int> target_range,
                                              PKB* pkb,
                                              const std::string& lhs_var);
  static bool TraverseScopedClusterForAffects(Cluster* scoped_cluster,
                                              std::pair<int, int> target_range,
                                              PKB* pkb,
                                              const std::string& lhs_var,
                                              std::pair<int, int> goal_range);
};

class Block : public Cluster {
 private:
  std::set<Block*> next_blocks_ = {};
  std::set<Block*> prev_blocks_ = {};

 public:
  Block() = default;;
  ~Block();
  static Block* GetNewExitBlock();
  static bool IsExitBlock(Block* block);
  static void PatchEmptyBlocks(Block* redundant, Block* to);
  void AddNextBlock(Block* next_block);
  std::set<Block*> GetNextBlocks() const;
  std::set<Block*> GetPrevBlocks() const;

  bool isWhile = false;
};

class ConditionalBlock : public Block {
  // todo: as an optimisation strat, see what can be kept in condi block to and body block
  //       to act as a negative list instead

 public:
  ~ConditionalBlock() override;
  ConditionalBlock() = default;;
};

class BodyBlock : public Block {

 public:
  ~BodyBlock() override;
  BodyBlock() = default;;
};

#endif //AUTOTESTER_SRC_SPA_SRC_MODEL_CFG_H_
