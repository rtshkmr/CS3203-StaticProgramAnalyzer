#include <util/Utility.h>
#include "AffectsExtractor.h"
#include "../../../model/CFG.h"

AffectsExtractor::AffectsExtractor(RuntimeMediator* rte, PKB* pkb) {
  this->rte_ = rte;
  this->pkb_ = pkb;
}

std::vector<Entity*> AffectsExtractor::GetRelationship(RelDirection dir, int target) {
  AssignEntity* target_ae = Utility::GetAssignEntityFromStmtNum(pkb_, target);

  if (!target_ae) return {};
  return (dir == RelDirection::kForward) ? GetAffects(target_ae) : GetAffectedBy(target_ae);
}

std::vector<Entity*> AffectsExtractor::GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) {
  if (!Utility::IsAssignDesignEntity(de))
    return std::vector<Entity*>{};

  return (dir == RelDirection::kForward) ? GetAllAffects() : GetAllAffectedBy();
}


std::vector<std::tuple<Entity*, Entity*>> AffectsExtractor::GetRelationshipByTypes(RelDirection dir,
                                                                                   DesignEntity first,
                                                                                   DesignEntity second) {
  std::vector<std::tuple<Entity*, Entity*>> retList = {};

  if (!Utility::IsAssignDesignEntity(first) || !Utility::IsAssignDesignEntity(second))
    return retList;

  if (cacheIndication == ScopeIndication::kAllScope) {
    for (auto pair : affects_map_) {
      std::vector<Entity*> this_key_entity = GetAffects(pair.first);
      for (auto val: this_key_entity) {
        retList.emplace_back(pair.first, val);
      }
    }
    return retList;
  }

  std::vector<Entity*> assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);
  for (auto* entity : assign_list) {
    auto* ae = dynamic_cast<AssignEntity*>(entity);
    std::vector<Entity*> all_affected_by_ae = GetAffects(ae);
    for (auto* affected : all_affected_by_ae) {
      retList.emplace_back(entity, affected);
    }
  }

  if (!retList.empty()) {
    retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  }
  cacheIndication = ScopeIndication::kAllScope;
  return retList;
}

std::pair<bool, bool> AffectsExtractor::CheckAgainstCache(AssignEntity* first_stmt, AssignEntity* second_stmt) {
  //check positive cache
  auto itr = affects_map_.find(first_stmt);
  if (itr != affects_map_.end()) {
    std::list<AssignEntity*>* values = itr->second;
    auto itr2 = std::find(values->begin(), values->end(), second_stmt);
    if (itr2 != values->end()) return {true, true};
  }
  //check negative cache
  itr = not_affects_map_.find(first_stmt);
  if (itr != not_affects_map_.end()) {
    std::list<AssignEntity*>* values = itr->second;
    auto itr2 = std::find(values->begin(), values->end(), second_stmt);
    if (itr2 != values->end()) return {true,false};
  }
  return {false, true};
}

bool AffectsExtractor::HasRelationship(RelDirection dir) {
  if(!affects_map_.empty()) return true;

  std::vector<Entity*> all_assign_list = pkb_->GetDesignEntities(DesignEntity::kAssign);

  for (auto* assign : all_assign_list) {
    auto* ae_to_check = dynamic_cast<AssignEntity*>(assign);
    if (HasAffects(ae_to_check)) {
      return true;
    }
  }
  return false;
}

bool AffectsExtractor::HasRelationship(RelDirection dir, int target) {
  AssignEntity* target_ae = Utility::GetAssignEntityFromStmtNum(pkb_, target);

  if (!target_ae) return false;
  return (dir == RelDirection::kForward) ? HasAffects(target_ae) : HasAffectedBy(target_ae);
}

bool AffectsExtractor::HasRelationship(RelDirection dir, int first, int second) {
  AssignEntity* ae_first = Utility::GetAssignEntityFromStmtNum(pkb_, first);
  AssignEntity* ae_second = Utility::GetAssignEntityFromStmtNum(pkb_, second);

  if (! ae_first || !ae_second) return false;
  return (dir == RelDirection::kForward) ? HasAffects(ae_first, ae_second) : HasAffects(ae_second, ae_first);
}

bool AffectsExtractor::HasRelationship(RelDirection dir, DesignEntity first, DesignEntity second) {
  if (!Utility::IsAssignDesignEntity(first) || !Utility::IsAssignDesignEntity(second)) return false;
  return HasRelationship(dir);
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
    auto* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffects(ae)) {
      retList.push_back(entity);
    }
  }

  if (!retList.empty()) {
    retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  }

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
    auto* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffectedBy(ae)) {
      retList.push_back(entity);
    }
  }

  if (!retList.empty()) {
    retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  }

  if (cacheIndication == ScopeIndication::kNoScope) {
    cacheIndication = ScopeIndication::kRightScope;
  } else if (cacheIndication == ScopeIndication::kLeftScope) {
    cacheIndication = ScopeIndication::kAllScope;
  }
  return retList;
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
    auto* ae = dynamic_cast<AssignEntity*>(entity);
    if (HasAffects(target, ae)) {
      retList.push_back(entity);
    }
  }

  if (!retList.empty()) {
    retList.erase(std::unique(retList.begin(), retList.end()), retList.end());
  }
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
    if (!values->empty()) {
      return true;
    } else if (cacheIndication == ScopeIndication::kAllScope || cacheIndication == ScopeIndication::kLeftScope) {
      return false;
    }
  }

  Procedure* target_proc = target_ae->GetProcedureNode();
  std::string target_var = target_ae->GetVariableString();

  //check against the entire procedure's assign
  std::vector<Entity*> affected_list =
      pkb_->GetRelationship(PKBRelRefs::kUsedBy, target_var);

  for (auto* assign : affected_list) {
    if (assign->GetEntityEnum() != EntityEnum::kAssignEntity) {
      continue;
    }

    auto* ae_to_check = dynamic_cast<AssignEntity*>(assign);
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
    if (!values->empty()) {
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
  auto cache_results = CheckAgainstCache(first_stmt, second_stmt);
  if(cache_results.first) return cache_results.second;

  bool are_affects_conditions_met = VerifyAffectsStmtConditions(first_stmt, second_stmt);
  if(!are_affects_conditions_met) return false;
  if (HasValidUnmodifiedPath(first_stmt, second_stmt)) {
    Deliverable::AddRelationshipToMap(&affects_map_, first_stmt, second_stmt);
    Deliverable::AddRelationshipToMap(&affected_by_map_, second_stmt, first_stmt);
    return true;
  } else {
    Deliverable::AddRelationshipToMap(&not_affects_map_, first_stmt, second_stmt);
    Deliverable::AddRelationshipToMap(&not_affected_by_map_, second_stmt, first_stmt);
    return false;
  }
}

/*
 * Verifies whether there exists a valid unmodified path between two assignment statements.
 *
 * */
bool AffectsExtractor::HasValidUnmodifiedPath(AssignEntity* first_stmt, AssignEntity* second_stmt) {
  Procedure* proc = first_stmt->GetProcedureNode();
  if (first_stmt->GetProcedureNode() != second_stmt->GetProcedureNode()) return false;
  int first_stmt_num = first_stmt->GetStatementNumber();
  int second_stmt_num = second_stmt->GetStatementNumber();
  std::vector<Entity*> proc_entities = this->pkb_->GetDesignEntities(DesignEntity::kProcedure);
  Cluster* scoped_cluster =  proc->GetInnermostCluster(first_stmt_num, second_stmt_num, nullptr);
  std::string lhs_var = first_stmt->GetVariableString();
  return Cluster::TraverseScopedCluster(PKBRelRefs::kAffects,
                                        scoped_cluster,
                                        std::make_pair(first_stmt_num, second_stmt_num),
                                        pkb_, lhs_var);
}

std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator> AffectsExtractor::GetPotentialAffectedBy(AssignEntity* target) {
  std::vector<Variable*> rhs_varlist = target->GetExprVariables();
  std::set<AssignEntity*, AffectsExtractor::AssignEntityComparator> affected_set = {};

  // Have a PQ per var.
  for (Variable* var: rhs_varlist) {
    auto* variable_name = const_cast<VariableName*>(var->GetVariableName());
    std::vector<Entity*> ent = pkb_->GetRelationship(PKBRelRefs::kModifiedBy, variable_name->GetName());

    //filter vector<Entity> into only AssignEntity as it can only take assign in LHS
    for (Entity* e: ent) {
      if (e->GetEntityEnum() == EntityEnum::kAssignEntity) {
        auto* ae = dynamic_cast<AssignEntity*>(e);
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

bool AffectsExtractor::VerifyAffectsStmtConditions(AssignEntity* first_stmt, AssignEntity* second_stmt) {
  // get the modified variable v from the lhs of first statement
  Variable* modified_var = first_stmt->GetVariableObj();
  std::vector<Variable*> vars_used_by_second_stmt =
      second_stmt->GetExprVariables();
  // check Uses(second_stmt, v)
  bool var_is_used = false;
  for (auto* var: vars_used_by_second_stmt) {
    if (modified_var == var) {
      var_is_used = true;
      break;
    }
  }
  return var_is_used;
}


