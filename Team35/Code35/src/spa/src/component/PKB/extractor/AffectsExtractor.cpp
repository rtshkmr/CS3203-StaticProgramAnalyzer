#include <queue>
#include "AffectsExtractor.h"

void AffectsExtractor::SetPKB(PKB* pkb) {
  this->pkb_ = pkb;
}

std::vector<Entity*> AffectsExtractor::GetAffects(int target) {
  return std::vector<Entity*>();
}

/**
 * Returns a list of Entity (ie. s1) that is: Affects(s1, target)
 */
std::vector<Entity*> AffectsExtractor::GetAffectedBy(int target) {
  std::vector<Entity*> return_list = {};

  std::string s = std::to_string(target);
  std::vector<Entity*> ae_vec_target = pkb_->GetAssignEntityByStmtRef(s);

  if (ae_vec_target.size() == 0) //target given is not an assign entity.
    return return_list;

  AssignEntity* assign_entity = dynamic_cast<AssignEntity*>(ae_vec_target[0]);

  //Get the var from RHS of assign
  std::vector<Variable*> rhs_varlist = assign_entity->GetControlVariables();

  // Have a PQ per var.
  for (Variable* var : rhs_varlist) {
    VariableName* variable_name = const_cast<VariableName*>(var->GetName());
    std::vector<Entity*> ent = pkb_->GetRelationship(PKBRelRefs::kModifiedBy, variable_name->getName());

    //filter vector<Entity> into only AssignEntity as it can only take assign in LHS
    auto cmp = [](AssignEntity* left, AssignEntity* right) { return left->GetStatementNumber()->GetNum() < right->GetStatementNumber()->GetNum(); };
    std::priority_queue<AssignEntity*, std::vector<AssignEntity*>, decltype(cmp)> queue(cmp);

    for (Entity* e : ent) {
      if (e->GetEntityEnum() == EntityEnum::kAssignEntity) {
        AssignEntity* ae = dynamic_cast<AssignEntity*>(e);
        queue.push(ae);
      }
    }

    while(queue.size() > 0) {
      AssignEntity* ae = queue.top();

      bool hasNextTRel = pkb_->HasRelationship(PKBRelRefs::kNextT,
                                               std::to_string(ae->GetStatementNumber()->GetNum()), std::to_string(target));

      //find if there is a Next* relationship
      if (hasNextTRel) {
        return_list.push_back(ae); //TODO: check for split-path eg. if/else
        break;
      }
      queue.pop();
    }
  }

  // Clear PQ
  return return_list;
}

void AffectsExtractor::Delete() {

}
