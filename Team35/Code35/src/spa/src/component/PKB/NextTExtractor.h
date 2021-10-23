#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

class NextTExtractor {
 public:
  int GetNextTSize();
  void Delete();
  std::vector<Entity*> GetNextT(std::string target,
                                std::vector<Procedure*> proc_list,
                                std::vector<Statement*> stmt_list);
  std::vector<Entity*> GetPrevT(std::string target,
                                std::vector<Procedure*> proc_list,
                                std::vector<Statement*> stmt_list);
  std::vector<Entity*> GetAllNextTLHS(std::vector<Procedure*> proc_list, std::vector<Statement*> stmt_list);
  std::vector<std::tuple<Entity*, Entity*>> GetAllNextT(std::vector<Procedure*> proc_list,
                                                        std::vector<Statement*> stmt_list);
 private:
  bool initialized_ = false;
  std::vector<Statement*> stmt_list_;

  // maps next* of stmt to list of stmts
  std::unordered_map<Statement*, std::list<Statement*>*> next_t_map_;
  std::vector<Entity*> next_t_lhs_stmts_;
  bool next_t_populated_ = false;
  // 2d array of first stmt (row) to second stmt (col). 1 indicates existence of next* relationship, 0 otherwise.
  std::vector<std::vector<int>> next_t_2d_array_;
  // stmt (index + 1) has next* relationship
  std::vector<int> next_t_array_;
  std::vector<std::tuple<Entity*, Entity*>> all_next_t_;
  bool got_all_next_t_ = false;

  // stmt has prev* relationship
  std::vector<int> has_prev_t_array_;
  std::unordered_map<Statement*, std::list<Statement*>*> previous_t_map_;

  void Init(const std::vector<Statement*> &stmt_list);
  std::list<Statement*> GetNextTFromCluster(Cluster* cluster, int target_num);
  static Cluster* GetTargetCluster(Cluster* p_cluster, int target_num);
  std::list<Statement*> GetNextTFromWhile(Cluster* w_cluster, int target_num);
  std::list<Statement*> GetNextTByTraversal(Block* block, int target_num);
  void AddNextT(Statement* s1, std::list<Statement*> s2);
  void AddNextTForIf(Statement* s1, std::list<Statement*> s2);
  static std::vector<Entity*> ltov(std::list<Statement*> l);
  void PopulateAllNextT(std::vector<Procedure*> proc_list);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
