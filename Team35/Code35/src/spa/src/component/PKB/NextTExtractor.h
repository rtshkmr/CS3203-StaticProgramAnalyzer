#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

class NextTExtractor {
 private:
  static std::unordered_map<Statement*, std::list<Statement*>*> next_t_map_;
  static std::unordered_map<Statement*, std::list<Statement*>*> previous_t_map_;
  static std::vector<Statement*>* stmt_list_;
 public:
  static std::vector<Entity*> GetNextT(std::string target,
                                       std::vector<Procedure*>* proc_list,
                                       std::vector<Statement*>* stmt_list);
  static std::vector<Entity*> GetNextTFromCluster(Cluster* cluster, int target_num);
  static std::vector<Entity*> GetNextTFromBlock(Block* block, int target_num);
  void AddNextTRelationship(Statement* s1, Statement* s2);
  void AddVectorOfNextTRelationship(Statement* s1, std::vector<Statement*> s2);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_NEXTTEXTRACTOR_H_
