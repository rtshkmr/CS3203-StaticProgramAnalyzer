#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

enum class RelDirection {
  kNext,
  kPrev
};

class NextTExtractor {
 public:
  int GetNextTSize();
  int GetPrevTSize();
  void Delete();
  std::vector<Entity*> GetNextT(int target,
                                const std::vector<Procedure*> &proc_list,
                                std::vector<Statement*> stmt_list);
  std::vector<Entity*> GetAllNextTLHS(const std::vector<Procedure*> &proc_list,
                                      const std::vector<Statement*> &stmt_list);
  std::vector<std::tuple<Entity*, Entity*>> GetAllNextT(const std::vector<Procedure*> &proc_list,
                                                        const std::vector<Statement*> &stmt_list);
  bool HasNextT(int first,
                int second,
                const std::vector<Procedure*> &proc_list,
                const std::vector<Statement*> &stmt_list);

  std::vector<Entity*> GetPrevT(int target,
                                const std::vector<Procedure*> &proc_list,
                                std::vector<Statement*> stmt_list);
  std::vector<Entity*> GetAllNextTRHS(const std::vector<Procedure*> &proc_list,
                                      const std::vector<Statement*> &stmt_list);
  std::vector<std::tuple<Entity*, Entity*>> GetAllPrevT(const std::vector<Procedure*> &proc_list,
                                                        const std::vector<Statement*> &stmt_list);
 private:
  std::vector<Statement*> stmt_list_;

  // state
  bool initialized_ = false;
  std::unordered_map<Statement*, std::list<Statement*>*>* map_to_populate_;
  std::vector<Entity*>* first_args_;
  RelDirection rel_direction_;

  std::unordered_map<Statement*, std::list<Statement*>*> next_t_map_; // maps next* of stmt to list of stmts
  std::vector<Entity*> next_t_lhs_stmts_;
  bool next_t_populated_ = false;
  // 2d array of first stmt (row) to second stmt (col). 1 indicates existence of next* relationship, 0 otherwise.
  std::vector<std::vector<int>> next_t_2d_array_;
  std::vector<std::tuple<Entity*, Entity*>> all_next_t_;
  bool got_all_next_prev_t_ = false;

  std::vector<Entity*> next_t_rhs_stmts_;
  std::unordered_map<Statement*, std::list<Statement*>*> prev_t_map_;
  bool prev_t_populated_ = false;
  std::vector<std::tuple<Entity*, Entity*>> all_prev_t_;  // reverse of all_next_t

  void Init(const std::vector<Statement*> &stmt_list);
  static Cluster* GetProcCluster(const std::vector<Procedure*> &proc_list, int target);
  std::list<Statement*> GetValueFromMap(std::unordered_map<Statement*, std::list<Statement*>*> map, int stmt_num);
  static Cluster* GetTargetCluster(Cluster* p_cluster, int target_num);
  static Block* GetNextBlockAfterWhile(Block* w_block);
  static std::vector<Entity*> ConvertListToVector(std::list<Statement*> list);
  void PopulateAllNextT(const std::vector<Procedure*> &proc_list);
  bool HasNextTInFirstCluster(Cluster* cluster, int first, int second);
  bool IsNextTDownstream(int first, int second);
  bool HasNextTInCluster(Cluster* cluster, int first, int second);
  bool HasNextTByTraversal(Block* block, int first, int second);
  std::list<Statement*>* MakeUniqueList(int s1_num, const std::list<Statement*> &list);
  void PopulateAllPrevT(const std::vector<Procedure*> &proc_list);
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
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
