#include <cassert>
#include <util/Utility.h>
#include "AffectsExtractor.h"
#include "../../../model/CFG.h"


void AffectsExtractor::SetPKB(PKB* pkb) {
  this->pkb_ = pkb;
}

/**
 * For Affects(_,_)
 */
bool AffectsExtractor::HasAffects() {
  if(affects_map_.size() > 0) return true;

  //TODO: Change optimised Affects(_,_)

  std::vector<Entity*> all_assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);

  for (auto* assign : all_assign_list) {
    AssignEntity* ae_to_check = dynamic_cast<AssignEntity*>(assign);
    if (HasAffects(ae_to_check)) {
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

  if (cacheIndication == ScopeIndication::kLeftScope || cacheIndication == ScopeIndication::kAllScope) {
    for (auto pair : affects_map_) {
      retList.push_back(pair.first);
    }
    return retList;
  }

  std::vector<Entity*> assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);
  for (auto* entity : assign_list) {
    AssignEntity* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffects(ae)) { //TODO change to Entity type?
      retList.push_back(entity);
    }
  }
  retList.erase(std::unique(retList.begin(), retList.end()), retList.end());

  if (cacheIndication == ScopeIndication::kNoScope) {
    cacheIndication = ScopeIndication::kLeftScope;
  } else if (cacheIndication == ScopeIndication::kRightScope) {
    cacheIndication = ScopeIndication::kAllScope;
  }
  return retList;
}

/**
 * For Affects(_, a1)
 */
std::vector<Entity*> AffectsExtractor::GetAllAffectedBy() {
  std::vector<Entity*> retList = {};

  if (cacheIndication == ScopeIndication::kRightScope || cacheIndication == ScopeIndication::kAllScope) {
    for (auto pair : affected_by_map_) {
      retList.push_back(pair.first);
    }
    return retList;
  }

  std::vector<Entity*> assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);
  for (auto* entity : assign_list) {
    AssignEntity* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffectedBy(ae)) {
      retList.push_back(entity);
    }
  }
  retList.erase(std::unique(retList.begin(), retList.end()), retList.end());

  if (cacheIndication == ScopeIndication::kNoScope) {
    cacheIndication = ScopeIndication::kRightScope;
  } else if (cacheIndication == ScopeIndication::kLeftScope) {
    cacheIndication = ScopeIndication::kAllScope;
  }
  return retList;
}

/**
 * For Affects(a1, a2)
 */
std::vector<std::tuple<Entity*, Entity*>> AffectsExtractor::GetAllPair() {
  std::vector<std::tuple<Entity*, Entity*>> retList = {};

  if (cacheIndication == ScopeIndication::kAllScope) {
    for (auto pair : affects_map_) {
      std::vector<Entity*> this_key_entity = GetAffects(pair.first);
      for (auto val: this_key_entity) {
        retList.push_back(std::make_tuple(pair.first, val));
      }
    }
    return retList;
  }

  std::vector<Entity*> assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);
  for (auto* entity : assign_list) {
    AssignEntity* ae = dynamic_cast<AssignEntity*>(entity);
    std::vector<Entity*> all_affected_by_ae = GetAffects(ae);
    for (auto* affected : all_affected_by_ae) {
      retList.push_back(std::make_tuple(entity, affected));
    }
  }
  retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  cacheIndication = ScopeIndication::kAllScope;
  return retList;
}

/**
 * Returns a vector of entities that the target affects. i.e. Affects(target, s)
 * @param target
 * @return
 */
std::vector<Entity*> AffectsExtractor::GetAffects(int target) {
  AssignEntity* target_ae = Utility::GetAssignEntityFromStmtNum(pkb_, target);
  return target_ae ? GetAffects(target_ae) : std::vector<Entity*>();
}

/**
 * Returns a list of Entity (ie. s1) that is: Affects(s1, target)
 */
std::vector<Entity*> AffectsExtractor::GetAffectedBy(int target) {
  AssignEntity* target_ae = Utility::GetAssignEntityFromStmtNum(pkb_, target);
  return target_ae ? GetAffectedBy(target_ae) : std::vector<Entity*>();
}

/**
 * For Affects(#,_)
 */
bool AffectsExtractor::HasAffects(int target) {
  AssignEntity* target_ae = Utility::GetAssignEntityFromStmtNum(pkb_, target);
  return target_ae ? HasAffects(target_ae) : false;
}

/**
 * For Affects(_,#)
 */
bool AffectsExtractor::HasAffectedBy(int target) {
  AssignEntity* target_ae = Utility::GetAssignEntityFromStmtNum(pkb_, target);
  return target_ae ? HasAffectedBy(target_ae) : false;
}

/**
 * Affects(#1, #2)
 */
bool AffectsExtractor::HasAffects(int first, int second) {
  AssignEntity* ae_first = Utility::GetAssignEntityFromStmtNum(pkb_, first);
  AssignEntity* ae_second = Utility::GetAssignEntityFromStmtNum(pkb_, second);
  return ae_first && ae_second ? HasAffects(ae_first, ae_second) : false;
}

bool AffectsExtractor::HasAffectedBy(int first, int second) {
  return HasAffects(second, first);
}

std::vector<Entity*> AffectsExtractor::GetAffects(AssignEntity* target) {
  std::vector<Entity*> retList = {};

  if (cacheIndication == ScopeIndication::kAllScope) {
    auto itr = affects_map_.find(target);
    if (itr != affects_map_.end()) {
      std::list<AssignEntity*>* values = itr->second;
      for (AssignEntity* val : *values) {
        retList.push_back(val);
      }
    }
    return retList;
  }

  std::vector<Entity*> assign_list = (cacheIndication == ScopeIndication::kRightScope)
      ? GetAllAffectedBy() : pkb_->GetDesignEntities(DesignEntity::kAssign);

  for (auto* entity : assign_list) {
    AssignEntity* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffects(target, ae)) {
      retList.push_back(entity);
    }
  }
  retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  return retList;
}

std::vector<Entity*> AffectsExtractor::GetAffectedBy(AssignEntity* target_assign_entity) {
  std::vector<Entity*> return_list = {};

  if (cacheIndication == ScopeIndication::kAllScope) {
    auto itr = affected_by_map_.find(target_assign_entity);
    if (itr != affected_by_map_.end()) {
      std::list<AssignEntity*>* values = itr->second;
      for (AssignEntity* val : *values) {
        return_list.push_back(val);
      }
    }
    return return_list;
  }


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
  auto itr = affects_map_.find(target_ae);
  if (itr != affects_map_.end()) {
    std::list<AssignEntity*>* values = itr->second;
    if (values->size() > 0) {
      return true;
    } else if (cacheIndication == ScopeIndication::kAllScope || cacheIndication == ScopeIndication::kLeftScope) {
      return false;
    }
  }

  Procedure* target_proc = target_ae->GetProcedureNode();
  std::string target_var = target_ae->GetVariableString();

  //check against the entire procedure's assign
  // TODO: create new PKB API, Given Procedure, Get all assign (Uses / Modifies)
  std::vector<Entity*> affected_list =
      pkb_->GetRelationship(PKBRelRefs::kUsedBy, target_var);

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

  auto iter = affected_by_map_.find(target_assign_entity);
  if (iter != affected_by_map_.end()) {
    std::list<AssignEntity*>* values = iter->second;
    if (values->size() > 0) {
      return true;
    } else if (cacheIndication == ScopeIndication::kAllScope || cacheIndication == ScopeIndication::kRightScope) {
      return false;
    }
  }

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

  //check positive cache
  auto itr = affects_map_.find(first_stmt);
  if (itr != affects_map_.end()) {
    std::list<AssignEntity*>* values = itr->second;
    auto itr2 = std::find(values->begin(), values->end(), second_stmt);
    if (itr2 != values->end()) return true;
  }

  //check negative cache
  itr = not_affects_map_.find(first_stmt);
  if (itr != not_affects_map_.end()) {
    std::list<AssignEntity*>* values = itr->second;
    auto itr2 = std::find(values->begin(), values->end(), second_stmt);
    if (itr2 != values->end()) return false;
  }

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

  bool ret = HasValidUnmodifiedPath(first_stmt, second_stmt);
  if (ret) {
    Deliverable::AddRelationshipToMap(&affects_map_, first_stmt, second_stmt);
    Deliverable::AddRelationshipToMap(&affected_by_map_, second_stmt, first_stmt);
  } else {
    Deliverable::AddRelationshipToMap(&not_affects_map_, first_stmt, second_stmt);
    Deliverable::AddRelationshipToMap(&not_affected_by_map_, second_stmt, first_stmt);
  }
  return ret;
}

/*
 * Verifies whether there exists a valid unmodified path between two assignment statements.
 *
 * */
bool AffectsExtractor::HasValidUnmodifiedPath(AssignEntity* first_stmt, AssignEntity* second_stmt) {
  Procedure* proc = first_stmt->GetProcedureNode();
  if (first_stmt->GetProcedureNode() != second_stmt->GetProcedureNode()) return false;
  int first_stmt_num = first_stmt->GetStatementNumberObj()->GetNum();
  int second_stmt_num = second_stmt->GetStatementNumberObj()->GetNum();
  std::vector<Entity*> proc_entities = this->pkb_->GetDesignEntities(DesignEntity::kProcedure);
  Cluster* scoped_cluster =  proc->GetInnermostCluster(first_stmt_num, second_stmt_num, nullptr);
  std::string lhs_var = first_stmt->GetVariableString();
  return Cluster::TraverseScopedCluster(PKBRelRefs::kAffects,
                                        scoped_cluster,
                                        std::make_pair(first_stmt_num, second_stmt_num),
                                        pkb_, lhs_var);
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
  this->cacheIndication = ScopeIndication::kNoScope;
  affects_map_ = {};
  affected_by_map_ = {};
  not_affects_map_ = {};
  not_affected_by_map_ = {};
}

