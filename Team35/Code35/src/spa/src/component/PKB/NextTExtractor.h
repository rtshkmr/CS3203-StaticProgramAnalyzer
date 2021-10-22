#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

class NextTExtractor {
 public:
  int GetNextTSize();
  std::vector<Entity*> GetNextT(std::string target,
                                       std::vector<Procedure*> proc_list,
                                       std::vector<Statement*> stmt_list);
  std::vector<Entity*> GetPrevT(std::string target,
                                std::vector<Procedure*> proc_list,
                                std::vector<Statement*> stmt_list);
  void Delete();
 private:
  bool initialized_ = false;
  std::vector<Statement*> stmt_list_;
  std::vector<Procedure*> proc_list_;
  std::unordered_map<Statement*, std::list<Statement*>*> next_t_map_;
  std::vector<std::vector<int>> next_t_array_;
  std::unordered_map<Statement*, std::list<Statement*>*> previous_t_map_;

  std::list<Statement*> GetNextTFromCluster(Cluster* cluster, int target_num);
  static Cluster* GetTargetCluster(Cluster* p_cluster, int target_num);
  std::list<Statement*> GetNextTFromWhile(Cluster* w_cluster, int target_num);
  std::list<Statement*> GetNextTByTraversal(Block* block, int target_num);
  void AddNextT(Statement* s1, std::list<Statement*> s2);
  void AddNextTForIf(Statement* s1, std::list<Statement*> s2);
  static std::vector<Entity*> ltov(std::list<Statement*> l);
  void Init();
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
