#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_

#include <model/Entity.h>
#include <component/PKB/PKB.h>
#include "RuntimeColleague.h"
#include "RuntimeMediator.h"

class NextBipExtractor : public RuntimeColleague {
 public:
  NextBipExtractor() = default;
  explicit NextBipExtractor(PKB* pkb);
  std::vector<Entity*> GetRelationship(RelDirection dir, int target) override;
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir,
                                                                   DesignEntity first,
                                                                   DesignEntity second) override;
  bool HasRelationship(RelDirection dir) override;
  bool HasRelationship(RelDirection dir, int target) override;
  bool HasRelationship(RelDirection dir, int first, int second) override;
  void Delete();
 private:
  PKB* pkb_{};
  std::vector<Entity*> stmt_list_;
  std::vector<Entity*> call_list_;
  std::unordered_map<Entity*, std::list<Entity*>*>* next_bip_map_{}; // maps nextbip of stmt num to list of stmts
  std::unordered_map<Entity*, std::list<Entity*>*>* prev_bip_map_{};

  void PopulateBipMaps();
  void JoinEndToEnd(Procedure* called_proc, const std::list<Entity*> &next_entities);
  void JoinStartToStart(Procedure* called_proc, Entity* prev_entity);
  std::list<Entity*> GetRelFromMap(Entity* target, PKBRelRefs rel);
  void AddNext(Entity* prev_entity, Entity* next_entity);
  static void AddRelationship(Entity* first_arg,
                              Entity* second_arg,
                              std::unordered_map<Entity*, std::list<Entity*>*>* map);
  void EraseNextRelationship(Entity* entity);
  void ErasePrevRelationship(Entity* next_stmt, Entity* prev_stmt);
  std::list<int> GetBipLastStmts(Block* block);
  std::list<int> HandleCallLastStmt(const std::list<int>& last_stmts);
  static PKBRelRefs GetPKBRelRef(RelDirection dir);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPEXTRACTOR_H_
