#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

class NextTExtractor {
 public:
  std::vector<Entity*> GetNextT(std::string target,
                                       std::vector<Procedure*> proc_list,
                                       std::vector<Statement*> stmt_list);
 private:
  std::unordered_map<Statement*, std::list<Statement*>*> next_t_map_;
  std::unordered_map<Statement*, std::list<Statement*>*> previous_t_map_;
  std::vector<Statement*> stmt_list_;

  std::list<Statement*> GetNextTFromCluster(Cluster* cluster, int target_num);
  Cluster* GetTargetCluster(Cluster* p_cluster, int target_num);
  std::list<Statement*> GetNextTFromWhile(Cluster* w_cluster, int target_num);
  std::list<Statement*> GetNextTByTraversal(Block* block, int target_num);
  void AddNextTRelationship(Statement* s1, Statement* s2);
  void AddVectorOfNextTRelationship(Statement* s1, std::vector<Statement*> s2);
  void AddListOfNextTRelationship(Statement* s1, std::list<Statement*> s2);
  static std::vector<Entity*> ltov(std::list<Statement*> l);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
