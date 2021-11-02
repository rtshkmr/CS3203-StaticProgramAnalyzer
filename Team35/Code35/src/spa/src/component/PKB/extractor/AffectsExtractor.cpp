#include <queue>
#include <cassert>
#include "AffectsExtractor.h"
#include "../../../model/CFG.h"


void AffectsExtractor::SetPKB(PKB* pkb) {
  this->pkb_ = pkb;
}

/**
 * Returns a vector of entities that the target affects. i.e. Affects(target, s)
 * @param target
 * @return
 */
std::vector<Entity*> AffectsExtractor::GetAffects(int target) {
  std::string target_string = std::to_string(target);

  // retrieve the entity that target points to
  // possible to have no such targets

  // assert that the target entity can only be an assignment statement

  // For each remaining stmt, x after target stmt, add to result vector if
  // HasAffects(target, x)

  return std::vector<Entity*>();
}

/**
 *  Returns true if the first stmt affects the second. This function shall be utilized by the GetX functions
 *  where we want valid vector of entities to be returned.
 * @param first_stmt
 * @param second_stmt
 * @return
 */
bool AffectsExtractor::HasAffects(AssignEntity* first_stmt, AssignEntity* second_stmt) {
  // get the modified variable v from the lhs of first statement
  Variable* modified_var = first_stmt->GetVariableObj();
  std::vector<Variable*> vars_used_by_second_stmt =
      second_stmt->GetControlVariables(); // todo: this is named wrongly, should be GetExpr Var for this
  // check Uses(second_stmt, v)
  bool var_is_used = false;
  for (auto* var: vars_used_by_second_stmt) {
    if (modified_var == var) {
      var_is_used = true;
      break;
    }
  }
  if (!var_is_used) return false;
  // check nextT, (just as a guarantee)
  return HasValidUnmodifiedPath(first_stmt, second_stmt);
}

/*
 * Verifies whether there exists a valid unmodified path between two assignment statements.
 *
 * */
bool AffectsExtractor::HasValidUnmodifiedPath(AssignEntity* first_stmt, AssignEntity* second_stmt) {
  int first_stmt_num = first_stmt->GetStatementNumber()->GetNum();
  int second_stmt_num = second_stmt->GetStatementNumber()->GetNum();
  std::vector<Entity*> proc_entities = this->pkb_->GetDesignEntities(DesignEntity::kProcedure);
  Cluster* scoped_cluster;
  for (auto entity: proc_entities) {
    auto* proc = dynamic_cast<Procedure*>(entity);
    assert(proc);
    scoped_cluster = proc->GetInnermostCluster(first_stmt_num, second_stmt_num, nullptr);
    if (scoped_cluster) break;
  }
  // call the boolean traversal helper function here, pass in the lhs argument that we're looking at
  std::string lhs_var = first_stmt->GetVariableString();
  return Cluster::TraverseScopedCluster(PKBRelRefs::kAffects,
                                        scoped_cluster,
                                        std::make_pair(first_stmt_num, second_stmt_num),
                                        pkb_, lhs_var);
}

/**
 * Returns a list of Entity (ie. s1) that is: Affects(s1, target)
 */
std::vector<Entity*> AffectsExtractor::GetAffectedBy(int target) {
  std::vector<Entity*> return_list = {};

  std::string s = std::to_string(target);
  std::vector<Entity*> ae_vec_target = pkb_->GetPatternEntities(DesignEntity::kAssign, s);

  if (ae_vec_target.size() == 0) //target given is not an assign entity.
    return return_list;

  AssignEntity* assign_entity = dynamic_cast<AssignEntity*>(ae_vec_target[0]);

  //Get the var from RHS of assign
  std::vector<Variable*> rhs_varlist = assign_entity->GetControlVariables();

  // Have a PQ per var.
  for (Variable* var: rhs_varlist) {
    VariableName* variable_name = const_cast<VariableName*>(var->GetVariableName());
    std::vector<Entity*> ent = pkb_->GetRelationship(PKBRelRefs::kModifiedBy, variable_name->GetName());

    //filter vector<Entity> into only AssignEntity as it can only take assign in LHS
    auto cmp = [](AssignEntity* left, AssignEntity* right) {
      return left->GetStatementNumber()->GetNum() < right->GetStatementNumber()->GetNum();
    };
    std::priority_queue<AssignEntity*, std::vector<AssignEntity*>, decltype(cmp)> queue(cmp);

    for (Entity* e: ent) {
      if (e->GetEntityEnum() == EntityEnum::kAssignEntity) {
        AssignEntity* ae = dynamic_cast<AssignEntity*>(e);
        queue.push(ae);
      }
    }

    while (queue.size() > 0) {
      AssignEntity* ae = queue.top();

      bool hasNextTRel = pkb_->HasRelationship(PKBRelRefs::kNextT,
                                               std::to_string(ae->GetStatementNumber()->GetNum()),
                                               std::to_string(target));

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

