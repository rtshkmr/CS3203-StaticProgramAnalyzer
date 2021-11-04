#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>
#include <component/PKB/extractor/RuntimeColleague.h>
#include <component/PKB/PKB.h>

class NextTExtractor : public RuntimeColleague {
 public:
  NextTExtractor() = default;
  explicit NextTExtractor(PKB* pkb);
  NextTExtractor(std::vector<Procedure*> proc_list, std::vector<Statement*> stmt_list);
  int GetNextTSize();
  int GetPrevTSize();
  void Delete();
  std::vector<Entity*> GetRelationship(RelDirection dir, int target) override;
  std::vector<Entity*> GetRelationship(RelDirection dir, int target, const std::vector<Procedure*> &proc_list);
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir,
                                                                   DesignEntity first,
                                                                   DesignEntity second) override;
  bool HasRelationship(RelDirection dir) override;
  bool HasRelationship(RelDirection dir, int target) override;
  bool HasRelationship(RelDirection dir, int first, int second) override;

 private:
  PKB* pkb_{};
  std::vector<Statement*> stmt_list_{};
  std::vector<Procedure*> proc_list_{};

  // state
  bool initialized_ = false;
  RelDirection rel_direction_;
  std::unordered_map<RelDirection, std::unordered_map<Statement*, std::list<Statement*>*>> rel_table_;
  std::unordered_map<RelDirection, std::unordered_map<DesignEntity, std::vector<Entity*>>> first_arg_table_;
  std::unordered_map<RelDirection,
                     std::unordered_map<type_combo, std::vector<entity_pair>, type_combo_hash>> rel_by_types_table_;

  bool next_t_populated_ = false;
  bool prev_t_populated_ = false;
  bool got_all_pairs_ = false;
  // 2d array of first stmt (row) to second stmt (col). 1 indicates existence of next* relationship, 0 otherwise.
  std::vector<std::vector<int>> next_t_2d_array_;

  static Cluster* GetProcCluster(const std::vector<Procedure*> &proc_list, int target);
  std::list<Statement*> GetValueFromMap(std::unordered_map<Statement*, std::list<Statement*>*> map, int stmt_num);
  static Cluster* GetTargetCluster(Cluster* p_cluster, int target_num);
  static Block* GetNextBlockAfterWhile(Block* w_block);
  static std::vector<Entity*> ConvertListToVector(std::list<Statement*> list);
  void PopulateRelationshipMap(const std::vector<Procedure*> &proc_list);
  bool HasNextTInFirstCluster(Cluster* cluster, int first, int second);
  bool IsNextTDownstream(int first, int second);
  bool HasNextTInCluster(Cluster* cluster, int first, int second);
  bool HasNextTByTraversal(Block* block, int first, int second);
  std::list<Statement*>* MakeUniqueList(int s1_num, const std::list<Statement*> &list);
  std::vector<Entity*> GetRel(int target, const std::vector<Procedure*> &proc_list, std::vector<Statement*> stmt_list);
  std::list<Statement*> GetRelFromCluster(Cluster* cluster, int target);
  std::list<Statement*> GetRelFromWhile(Cluster* w_cluster, int target);
  std::list<Statement*> GetRelByTraversal(Block* block, int target);
  void AddRelationships(Statement* first_arg, std::list<Statement*> second_args);
  void AddRelationshipsWithDup(Statement* first_arg, const std::list<Statement*> &second_args);
  std::list<Statement*> RecurseFollowingBlocks(Block* block, int target);
  std::list<Statement*> AddBlockStmtToRelList(std::list<Statement*> rel_list, Block* block);
  std::set<Block*> GetFollowingBlocks(Block* block);
  std::list<Block*> GetFollowingBlocksAfterWhile(Block* block);
  void AddRelationshipsInBlock(std::list<Statement*> rel_stmts, Block* block, int target);
  void AddRelationshipsFollowingBlock(const std::list<Statement*> &rel_stmts, Block* block);
  int GetBlockTarget(Block* block, int target);
  std::list<Statement*> MakeStmtList(int first_stmt, int last_stmt);
  void AddRelByTypes(RelDirection dir, Entity* first_arg, Entity* second_arg);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
