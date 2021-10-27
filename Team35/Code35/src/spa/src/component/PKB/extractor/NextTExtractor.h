#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

class NextTExtractor {
 public:
  int GetNextTSize();
  int GetPrevTSize();
  void Delete();
  std::vector<Entity*> GetNextT(int target,
                                std::vector<Procedure*> proc_list,
                                std::vector<Statement*> stmt_list);
  std::vector<Entity*> GetAllNextTLHS(std::vector<Procedure*> proc_list, std::vector<Statement*> stmt_list);
  std::vector<std::tuple<Entity*, Entity*>> GetAllNextT(std::vector<Procedure*> proc_list,
                                                        std::vector<Statement*> stmt_list);
  bool HasNextT(int first, int second, std::vector<Procedure*> proc_list, std::vector<Statement*> stmt_list);

  std::vector<Entity*> GetPrevT(int target,
                                std::vector<Procedure*> proc_list,
                                std::vector<Statement*> stmt_list);
  std::vector<Entity*> GetAllNextTRHS(std::vector<Procedure*> proc_list, std::vector<Statement*> stmt_list);
  std::vector<std::tuple<Entity*, Entity*>> GetAllPrevT(std::vector<Procedure*> proc_list,
                                                        std::vector<Statement*> stmt_list);
 private:
  bool initialized_ = false;
  std::vector<Statement*> stmt_list_;

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
  std::list<Statement*> GetNextTFromCluster(Cluster* cluster, int target_num);
  Cluster* GetProcCluster(std::vector<Procedure*> proc_list, int target);
  std::list<Statement*> GetValueFromMap(std::unordered_map<Statement*, std::list<Statement*>*> map, int stmt_num);
  static Cluster* GetTargetCluster(Cluster* p_cluster, int target_num);
  std::list<Statement*> GetNextTFromWhile(Cluster* w_cluster, int target_num);
  static Block* GetNextBlockAfterWhile(Block* w_block);
  std::list<Statement*> GetNextTByTraversal(Block* block, int target_num);
  std::list<Statement*> RecurseNextBlocks(Block* block, int target_num);
  void AddNextT(Statement* s1, std::list<Statement*> s2);
  void AddNextTWithDup(Statement* s1, std::list<Statement*> s2);
  static std::vector<Entity*> ConvertListToVector(std::list<Statement*> list);
  void PopulateAllNextT(std::vector<Procedure*> proc_list);
  bool HasNextTInFirstCluster(Cluster* cluster, int first, int second);
  bool IsNextTDownstream(int first, int second);
  bool HasNextTInCluster(Cluster* cluster, int first, int second);
  bool HasNextTByTraversal(Block* block, int first, int second);
  std::list<Statement*> GetPrevTFromCluster(Cluster* cluster, int target);
  std::list<Statement*> GetPrevTFromWhile(Cluster* w_cluster, int target_num);
  static std::list<Block*> GetPrevBlocksBeforeWhile(Block* w_block);
  std::list<Statement*> GetPrevTByTraversal(Block* block, int target_num);
  std::list<Statement*> RecursePrevBlocks(Block* block, int target_num);
  void AddPrevT(Statement* s1, std::list<Statement*> s2);
  void AddPrevTWithDup(Statement* s1, std::list<Statement*> s2);
  std::list<Statement*>* MakeUniqueList(int s1_num, std::list<Statement*> list);
  void PopulateAllPrevT(std::vector<Procedure*> proc_list);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
