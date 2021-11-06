#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>
#include <component/PKB/PKB.h>
#include "RuntimeMediator.h"
#include "RuntimeColleague.h"

class AffectsExtractor : public RuntimeColleague {
  struct AssignEntityComparator
  {
    bool operator()(AssignEntity* lhs, AssignEntity* rhs) const  {
      int left = lhs->GetStatementNumber();
      int right = rhs->GetStatementNumber();
      return left > right;
    }
  };
 public:
  AffectsExtractor() = default;
  explicit AffectsExtractor(RuntimeMediator* rte, PKB* pkb);

  std::vector<Entity*> GetRelationship(RelDirection dir, int target);
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de);
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir,
                                                                   DesignEntity first,
                                                                   DesignEntity second);
  bool HasRelationship(RelDirection dir);
  bool HasRelationship(RelDirection dir, int target);
  bool HasRelationship(RelDirection dir, int first, int second);

  void Delete();
 private:
  RuntimeMediator* rte_;
  PKB* pkb_;
  static constexpr auto cmp = [](AssignEntity* left, AssignEntity* right) {
    return left->GetStatementNumber() < right->GetStatementNumber();
  };
  ScopeIndication cacheIndication = ScopeIndication::kNoScope;
  std::unordered_map<AssignEntity*, std::list<AssignEntity*>*> affects_map_;
  std::unordered_map<AssignEntity*, std::list<AssignEntity*>*> affected_by_map_;
  std::unordered_map<AssignEntity*, std::list<AssignEntity*>*> not_affects_map_;
  std::unordered_map<AssignEntity*, std::list<AssignEntity*>*> not_affected_by_map_;
  bool HasValidUnmodifiedPath(AssignEntity* first_stmt, AssignEntity* second_stmt);
  std::set<AssignEntity*, AssignEntityComparator> GetPotentialAffectedBy(AssignEntity* target);
  std::vector<Entity*> GetAllAffects();
  std::vector<Entity*> GetAllAffectedBy();
  std::vector<Entity*> GetAffects(AssignEntity* target);
  std::vector<Entity*> GetAffectedBy(AssignEntity* target);
  bool HasAffects(AssignEntity* target);
  bool HasAffectedBy(AssignEntity* target);
  bool HasAffects(AssignEntity* first_stmt, AssignEntity* second_stmt);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_
