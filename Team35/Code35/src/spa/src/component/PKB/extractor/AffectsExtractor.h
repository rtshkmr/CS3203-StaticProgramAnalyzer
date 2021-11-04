#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>
#include <component/PKB/PKB.h>

class AffectsExtractor {
  struct AssignEntityComparator
  {
    bool operator()(AssignEntity* lhs, AssignEntity* rhs) const  {
      int left = lhs->GetStatementNumber()->GetNum();
      int right = rhs->GetStatementNumber()->GetNum();
      return left > right;
    }
  };
 public:
  bool HasAffects();
  std::vector<Entity*> GetAllAffects();
  std::vector<Entity*> GetAllAffectedBy();
  std::vector<std::tuple<Entity*, Entity*>> GetAllPair();

  std::vector<Entity*> GetAffects(int target);
  std::vector<Entity*> GetAffectedBy(int target);
  bool HasAffects(int target);
  bool HasAffectedBy(int target);
  bool HasAffects(int first, int second);
  bool HasAffectedBy(int first, int second);

  std::vector<Entity*> GetAffects(AssignEntity* target);
  std::vector<Entity*> GetAffectedBy(AssignEntity* target);
  bool HasAffects(AssignEntity* target);
  bool HasAffectedBy(AssignEntity* target);
  bool HasAffects(AssignEntity* first_stmt, AssignEntity* second_stmt); //also used by HasAffectedBy(second, first)

  void SetPKB(PKB* pkb);
  void Delete();
 private:
  PKB* pkb_;
  static constexpr auto cmp = [](AssignEntity* left, AssignEntity* right) {
    return left->GetStatementNumber()->GetNum() < right->GetStatementNumber()->GetNum();
  };
  std::unordered_map<Statement*, std::list<Statement*>*> affects_map_;
  std::unordered_map<Statement*, std::list<Statement*>*> affected_by_map_;
  bool HasValidUnmodifiedPath(AssignEntity* first_stmt, AssignEntity* second_stmt);
  std::set<AssignEntity*, AssignEntityComparator> GetPotentialAffectedBy(AssignEntity* target);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_
