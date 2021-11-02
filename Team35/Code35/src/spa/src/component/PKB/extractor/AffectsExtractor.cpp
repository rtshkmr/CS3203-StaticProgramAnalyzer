#include <cassert>
#include "AffectsExtractor.h"
#include "../../../model/CFG.h"


void AffectsExtractor::SetPKB(PKB* pkb) {
  this->pkb_ = pkb;
}

/**
 * For Affects(_,_)
 */
bool AffectsExtractor::HasAffects() {
  //TODO: Check cache size > 0;

  //TODO: Change optimised Affects(_,_)

  std::vector<Entity*> all_assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);

  for (auto* assign : all_assign_list) {
    AssignEntity* ae_to_check = dynamic_cast<AssignEntity*>(assign); // TODO: make more overloading methods;
    if (HasAffects(ae_to_check->GetStatementNumber()->GetNum())) {
      return true;
    }
  }
  return false;
}

/**
 * For Affects(a1, _)
 */
std::vector<Entity*> AffectsExtractor::GetAllAffects() {
  std::vector<Entity*> retList = {};

  std::vector<Entity*> assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);
  for (auto* entity : assign_list) {
    AssignEntity* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffects(ae->GetStatementNumber()->GetNum())) { //TODO change to Entity type?
      retList.push_back(entity);
    }
  }
  retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  return retList;
}

/**
 * For Affects(_, a1)
 */
std::vector<Entity*> AffectsExtractor::GetAllAffectedBy() {
  std::vector<Entity*> retList = {};

  std::vector<Entity*> assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);
  for (auto* entity : assign_list) {
    AssignEntity* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffectedBy(ae->GetStatementNumber()->GetNum())) { //TODO change to Entity type?
      retList.push_back(entity);
    }
  }
  retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  return retList;
}

/**
 * For Affects(a1, a2)
 */
std::vector<std::tuple<Entity*, Entity*>> AffectsExtractor::GetAllPair() {
  std::vector<std::tuple<Entity*, Entity*>> retList = {};

  std::vector<Entity*> assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);
  for (auto* entity : assign_list) {
    AssignEntity* ae = dynamic_cast<AssignEntity*>(entity);
    std::vector<Entity*> all_affected_by_ae = GetAffects(ae->GetStatementNumber()->GetNum());
    for (auto* affected : all_affected_by_ae) {
      retList.push_back(std::make_tuple(entity, affected));
    }
  }
  retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  return retList;
}

/**
 * Returns a vector of entities that the target affects. i.e. Affects(target, s)
 * @param target
 * @return
 */
std::vector<Entity*> AffectsExtractor::GetAffects(int target) {
  AssignEntity* target_ae = GetAssignEntityFromStmtNum(target);
  return target_ae ? GetAffects(target_ae) : std::vector<Entity*>();
}

/**
 * Returns a list of Entity (ie. s1) that is: Affects(s1, target)
 */
std::vector<Entity*> AffectsExtractor::GetAffectedBy(int target) {
  AssignEntity* target_ae = GetAssignEntityFromStmtNum(target);
  return target_ae ? GetAffectedBy(target_ae) : std::vector<Entity*>();
}

/**
 * For Affects(#,_)
 */
bool AffectsExtractor::HasAffects(int target) {
  AssignEntity* target_ae = GetAssignEntityFromStmtNum(target);
  return target_ae ? HasAffects(target_ae) : false;
}

/**
 * For Affects(_,#)
 */
bool AffectsExtractor::HasAffectedBy(int target) {
  AssignEntity* target_ae = GetAssignEntityFromStmtNum(target);
  return target_ae ? HasAffectedBy(target_ae) : false;
}

/**
 * Affects(#1, #2)
 */
bool AffectsExtractor::HasAffects(int first, int second) {
  AssignEntity* ae_first = GetAssignEntityFromStmtNum(first);
  AssignEntity* ae_second = GetAssignEntityFromStmtNum(second);
  return ae_first && ae_second ? HasAffects(ae_first, ae_second) : false;
}

bool AffectsExtractor::HasAffectedBy(int first, int second) {
  return HasAffects(second, first);
}

std::vector<Entity*> AffectsExtractor::GetAffects(AssignEntity* target) {

  // retrieve the entity that target points to
  // possible to have no such targets

  // assert that the target entity can only be an assignment statement

  // For each remaining stmt, x after target stmt, add to result vector if
  // HasAffects(target, x)

  return std::vector<Entity*>();
}

std::vector<Entity*> AffectsExtractor::GetAffectedBy(AssignEntity* target_assign_entity) {
  std::vector<Entity*> return_list = {};

  std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator> affected_set = GetPotentialAffectedBy(target_assign_entity);

  std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator>::iterator itr;
  for (itr = affected_set.begin(); itr != affected_set.end(); itr++) {
    if (HasAffects(*itr, target_assign_entity)) {
      return_list.push_back(*itr);
    }
  }

  // Clear PQ
  return return_list;
}

bool AffectsExtractor::HasAffects(AssignEntity* target_ae) {
  Procedure* target_proc = target_ae->GetProcedureNode();
//  VariableName* target_var = const_cast<VariableName*>(target_ae->GetVariableObj()yyp->GetName());
  VariableName* target_var = reinterpret_cast<VariableName*>(target_ae->GetVariableObj());

  //check against the entire procedure's assign
  // TODO: create new PKB API, Given Procedure, Get all assign (Uses / Modifies)
  std::vector<Entity*> affected_list =
      pkb_->GetPatternEntities(DesignEntity::kVariable, target_var->GetName());

  for (auto* assign : affected_list) {
    if (assign->GetEntityEnum() != EntityEnum::kAssignEntity) {
      continue;
    }

    AssignEntity* ae_to_check = dynamic_cast<AssignEntity*>(assign);
    if (ae_to_check->GetProcedureNode() != target_proc) {
      continue;
    }

    if (HasAffects(target_ae, ae_to_check)) {
      return true;
    }
  }
  return false;
}

bool AffectsExtractor::HasAffectedBy(AssignEntity* target_assign_entity) {
  std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator> affected_set = GetPotentialAffectedBy(target_assign_entity);

  std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator>::iterator itr;
  for (itr = affected_set.begin(); itr != affected_set.end(); itr++) {
    if (HasAffects(*itr, target_assign_entity)) {
      return true;
    }
  }

  return false;
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

AssignEntity* AffectsExtractor::GetAssignEntityFromStmtNum(int target) {
  std::vector<Entity*> ae_vec_target =
      pkb_->GetPatternEntities(DesignEntity::kAssign, std::to_string(target));

  if (ae_vec_target.size() == 0) //target given is not an assign entity.
    return nullptr;

  assert (ae_vec_target.size() == 1); // must be 1
  return dynamic_cast<AssignEntity*>(ae_vec_target[0]);
}

std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator> AffectsExtractor::GetPotentialAffectedBy(AssignEntity* target) {
  std::vector<Variable*> rhs_varlist = target->GetControlVariables();
  std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator> affected_set = {};

  // Have a PQ per var.
  for (Variable* var: rhs_varlist) {
    VariableName* variable_name = const_cast<VariableName*>(var->GetVariableName());
    std::vector<Entity*> ent = pkb_->GetRelationship(PKBRelRefs::kModifiedBy, variable_name->GetName());

    //filter vector<Entity> into only AssignEntity as it can only take assign in LHS
    for (Entity* e: ent) {
      if (e->GetEntityEnum() == EntityEnum::kAssignEntity) {
        AssignEntity* ae = dynamic_cast<AssignEntity*>(e);
        if (ae->GetProcedureNode() == target->GetProcedureNode()) {
          affected_set.insert(ae);
        }
      }
    }
  }

  return affected_set;
}

void AffectsExtractor::Delete() {

}

