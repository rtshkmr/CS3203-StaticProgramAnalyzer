#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_

#include <model/Entity.h>
#include <component/PKB/PKB.h>

enum class NextBipRel {
  kNextBip,
  kPrevBip
};

class NextBipExtractor {
 public:
  NextBipExtractor() = default;
  explicit NextBipExtractor(PKB* pkb);
  std::vector<Entity*> GetNextBip(int target);
  std::vector<Entity*> GetPrevBip(int target);
  std::vector<Entity*> GetAllNextBipRHS();
  std::vector<Entity*> GetAllNextBipLHS();
 private:
  PKB* pkb_{};
  std::vector<Entity*> stmt_list_;
  std::vector<Procedure*> proc_list_;
  std::vector<Entity*> call_list_;
  std::unordered_map<int, std::vector<Entity*>*> next_bip_map_; // maps nextbip of stmt num to list of stmts
  std::unordered_map<int, std::vector<Entity*>*> prev_bip_map_;

  void PopulateBipMaps(std::unordered_map<int, std::vector<Entity*>*> next_map,
                       std::unordered_map<int, std::vector<Entity*>*> prev_map);
  void JoinEndToEnd(Procedure* called_proc, const std::vector<Entity*> &next_entities);
  void JoinStartToStart(Procedure* called_proc, Entity* prev_entity);
  void AddNext(Entity* prev_entity, Entity* next_entity);
  static void AddRelationship(Entity* first_arg,
                              Entity* second_arg,
                              std::unordered_map<int, std::vector<Entity*>*> map);
  static int GetStmtNum(Entity* entity);
  void EraseNextRelationship(Entity* entity);
  void ErasePrevRelationship(Entity* next_stmt, Entity* prev_stmt);
  std::vector<Entity*> GetRelFromMap(int target, NextBipRel rel);
  std::vector<Entity*> GetRel(NextBipRel rel, int target);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_
