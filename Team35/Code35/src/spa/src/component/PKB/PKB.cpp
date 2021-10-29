#include <exception/SpaException.h>
#include "PKB.h"
#include <stdio.h>
#include <iostream>
#include <util/Logger.h>

constexpr auto L = [](auto msg) {
  LOG
  (spa_logger << Logger::Prettify(msg));
};

/**
 * Populates the respective entity lists and relationship hashmaps
 * @param d is a Deliverable object initialised in PSubsystem
 * @return
 */
void PKB::PopulateDataStructures(Deliverable d) {
  L("... PKB will be populated by Deliverable object from SourceProcessor\n");

  PopulateEntities(DesignEntity::kProcedure, *d.GetProcList());
  PopulateEntities(DesignEntity::kVariable, *d.GetVariableList());
  PopulateEntities(DesignEntity::kConstant, *d.GetConstantList());
  PopulateEntities(DesignEntity::kCall, *d.GetCallList());
  PopulateEntities(DesignEntity::kPrint, *d.GetPrintList());
  PopulateEntities(DesignEntity::kRead, *d.GetReadList());
  PopulateEntities(DesignEntity::kIf, *d.GetIfList());
  PopulateEntities(DesignEntity::kWhile, *d.GetWhileList());
  PopulateEntities(DesignEntity::kAssign, *d.GetAssignList());
  PopulateEntities(DesignEntity::kStmt, *d.GetStatementList());
  
  PopulateRelationship(d.GetFollowsMap(), PKBRelRefs::kFollows);
  PopulateRelationship(d.GetFollowedByMap(), PKBRelRefs::kFollowedBy);
  PopulateRelationship(d.GetFollowsTMap(), PKBRelRefs::kFollowsT);
  PopulateRelationship(d.GetFollowedByTMap(), PKBRelRefs::kFollowedByT);
  PopulateRelationship(d.GetParentMap(), PKBRelRefs::kParent);
  PopulateRelationship(d.GetParentTMap(), PKBRelRefs::kParentT);
  PopulateRelationship(d.GetChildMap(), PKBRelRefs::kChild);
  PopulateRelationship(d.GetChildTMap(), PKBRelRefs::kChildT);
  PopulateRelationship(d.GetUseSMap(), PKBRelRefs::kUsesS);
  PopulateRelationship(d.GetUsedBySMap(), PKBRelRefs::kUsedByS);
  PopulateRelationship(d.GetModifiesSMap(), PKBRelRefs::kModifiesStatement);
  PopulateRelationship(d.GetModifiedBySMap(), PKBRelRefs::kModifiedByStatement);
  PopulateRelationship(d.GetCallsMap(), PKBRelRefs::kCalls);
  PopulateRelationship(d.GetCalledByMap(), PKBRelRefs::kCalledBy);
  PopulateRelationship(d.GetCallsTMap(), PKBRelRefs::kCallsT);
  PopulateRelationship(d.GetCalledByTMap(), PKBRelRefs::kCalledByT);
  PopulateRelationship(d.GetNextMap(), PKBRelRefs::kNext);
  PopulateRelationship(d.GetPrevMap(), PKBRelRefs::kPrevious);

  PopulateContainerUse(*d.GetUseCMap());
  PopulateContainerUsedBy(*d.GetUsedByCMap());
  PopulateContainerModifies(*d.GetModifiesCMap());
  PopulateContainerModifiedBy(*d.GetModifiedByCMap());

  PopulateUses();
  PopulateModifies();
  PopulateUsedBy();
  PopulateModifiedBy();

  ProcessEntitiesWithMatchingAttributes();

  L("[DONE] PKB POPULATED WITHOUT ERROR\n");
}

template <typename T>
void PKB::PopulateEntities(DesignEntity design_entity, T& entity_list) {
  for (auto entity : entity_list) {
    type_to_entity_map_[design_entity].push_back(entity);
    std::string entity_name = GetNameFromEntity(entity);
    Attribute attribute = GetAttributeFromEntity(entity);
    attribute_to_entity_map_[{design_entity, attribute}][entity_name].push_back(entity);
    attribute_string_to_entity_map_[entity_name].insert(entity);
    if (pattern_entities_.find(design_entity) != pattern_entities_.end()) {
      pattern_maps_[design_entity][entity_name].push_back(entity);

      std::vector<Variable*> pattern_variables;

      Entity* casted_entity = reinterpret_cast<Entity*>(entity);
      EntityEnum entity_enum = casted_entity->GetEntityEnum();

      switch (entity_enum) {
        case EntityEnum::kWhileEntity: {
          WhileEntity* while_entity = reinterpret_cast<WhileEntity*>(entity);
          pattern_variables = while_entity->GetControlVariables();
        } break;
        case EntityEnum::kIfEntity: {
          IfEntity* if_entity = reinterpret_cast<IfEntity*>(entity);
          pattern_variables = if_entity->GetControlVariables();
        } break;
        case EntityEnum::kAssignEntity: {
          AssignEntity* assign_entity = reinterpret_cast<AssignEntity*>(entity);
          pattern_variables.push_back(assign_entity->GetVariable());
        } break;
        default: {
          throw PKBException("Invalid EntityEnum in pattern entity.");
        } break;
      }

      for (Variable* variable : pattern_variables) {
        pattern_maps_[design_entity][GetNameFromEntity(variable)].push_back(entity);
      }
    }
  }
}

template <typename X, typename Y>
void PKB::PopulateRelationship(std::unordered_map<X*, std::list<Y*>*>* hash, PKBRelRefs ref) {
  for (std::pair<X*, std::list<Y*>*> kv: *hash) {
    std::string k_string = GetNameFromEntity(kv.first);
    DesignEntity first_type = GetDesignEntityFromEntity(kv.first);
    std::vector<DesignEntity> first_types = GetApplicableTypes(first_type);
    std::vector<DesignEntity> second_types;

    for (Entity* entity : *kv.second) {
      relationship_set_.insert({ref, k_string, GetNameFromEntity(entity)});
      relationship_table_[ref][k_string].push_back(entity);

      DesignEntity second_type = GetDesignEntityFromEntity(entity);
      second_types = GetApplicableTypes(second_type);


      for (DesignEntity type1 : first_types) {
        for (DesignEntity type2 : second_types) {
          relationship_by_types_table_[ref][{type1, type2}].push_back({kv.first, entity});
          first_param_by_types_table_[ref][{type1, type2}].push_back(kv.first);
        }
      }
    }
  }
}

std::vector<Entity*> PKB::GetRelationship(PKBRelRefs ref, std::string entity) {
    return relationship_table_[ref][entity];
}

std::vector<std::tuple<Entity*, Entity*>> PKB::GetRelationshipByTypes(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
    if (d1 == DesignEntity::kProgLine) {
      d1 = DesignEntity::kStmt;
    }
    if (d2 == DesignEntity::kProgLine) {
      d2 = DesignEntity::kStmt;
    }

    return relationship_by_types_table_[ref][{d1, d2}];
}

std::vector<Entity*> PKB::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
  if (d1 == DesignEntity::kProgLine) {
    d1 = DesignEntity::kStmt;
  }
  if (d2 == DesignEntity::kProgLine) {
    d2 = DesignEntity::kStmt;
  }

  return first_param_by_types_table_[ref][{d1, d2}];
}

std::vector<Entity*> PKB::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity d) {
    if (d == DesignEntity::kProgLine) {
      d = DesignEntity::kStmt;
    }
    if (second_param_is_stmt.find(ref) != second_param_is_stmt.end()) {
      return first_param_by_types_table_[ref][{d, DesignEntity::kStmt}];
    } else {
      if (second_param_is_var.find(ref) != second_param_is_var.end()) {
        return first_param_by_types_table_[ref][{d, DesignEntity::kVariable}];
      } else if (second_param_is_proc.find(ref) != second_param_is_proc.end()) {
        return first_param_by_types_table_[ref][{d, DesignEntity::kProcedure}];
      }
    }
}

std::vector<Entity*> PKB::GetDesignEntities(DesignEntity de) {
    if (de == DesignEntity::kProgLine) {
      de = DesignEntity::kStmt;
    }
    return type_to_entity_map_[de];
}

std::vector<Entity*> PKB::GetPatternEntities(DesignEntity de, std::string var_or_stmt) {
  return pattern_maps_[de][var_or_stmt];
}

std::vector<Entity*> PKB::GetEntitiesWithAttributeValue(DesignEntity design_entity, Attribute attribute, std::string value) {
  return attribute_to_entity_map_[{design_entity, attribute}][value];
}

std::vector<entity_pair> PKB::GetEntitiesWithMatchingAttributes(DesignEntity type_one, DesignEntity type_two) {
  return entities_with_matching_attributes_map_[{type_one, type_two}];
}

bool PKB::HasRelationship(PKBRelRefs ref) {
  return !relationship_table_[ref].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
  return !relationship_by_types_table_[ref][{d1, d2}].empty();
}

// todo: see if last part of function can be replaced with return !rel_map[e].empty();
bool PKB::HasRelationship(PKBRelRefs ref, std::string e) {
  if (!HasRelationship(ref)) {
    return false;
  } else {
    std::unordered_map<std::string, std::vector<Entity*>> rel_map = relationship_table_[ref];
    if (rel_map.find(e) != rel_map.end()) {
      return !rel_map[e].empty();
    } else {
      return false;
    }
  }
}

bool PKB::HasRelationship(PKBRelRefs ref, std::string e1, std::string e2) {
  return relationship_set_.find({ref, e1, e2}) != relationship_set_.end();
}

void PKB::ProcessEntitiesWithMatchingAttributes() {
  for (std::pair<std::string, std::unordered_set<Entity*>> kv : attribute_string_to_entity_map_) {
    for (Entity* entity_one : kv.second) {
      DesignEntity design_entity_one = GetDesignEntityFromEntity(entity_one);
      for (Entity* entity_two : kv.second) {
        DesignEntity design_entity_two = GetDesignEntityFromEntity(entity_two);
        entities_with_matching_attributes_map_[{design_entity_one, design_entity_two}]
          .push_back({entity_one, entity_two});
      }
    }
  }
}



// todo: See if can optimize this
std::string PKB::GetNameFromEntity(Entity* entity) {
    if (dynamic_cast<Procedure*>(entity) != nullptr) {
        Procedure* proc = dynamic_cast<Procedure*>(entity);
        ProcedureName* proc_name = const_cast<ProcedureName*>(proc->GetName());
        return proc_name->getName();
    } else if (dynamic_cast<Statement*>(entity) != nullptr) {
        Statement* stmt = dynamic_cast<Statement*>(entity);
        auto* k_number = const_cast<StatementNumber*>(stmt->GetStatementNumber());
        return std::to_string(k_number->GetNum());
    } else if (dynamic_cast<Variable*>(entity) != nullptr) {
        Variable* var = dynamic_cast<Variable*>(entity);
        VariableName* variable_name = const_cast<VariableName*>(var->GetName());
        return variable_name->getName();
    } else if (dynamic_cast<Constant*>(entity) != nullptr) {
        Constant* constant = dynamic_cast<Constant*>(entity);
        ConstantValue* cv = const_cast<ConstantValue*>(constant->GetValue());
        return std::to_string(cv->Get());
    } else {
      throw PKBException("Invalid entity type encountered.");
    }
}


DesignEntity PKB::GetDesignEntityFromEntity(Entity* entity) {
    EntityEnum entity_enum = entity->GetEntityEnum();
    switch (entity_enum) {
        case (EntityEnum::kIfEntity):
            return DesignEntity::kIf;
        case (EntityEnum::kWhileEntity):
            return DesignEntity::kWhile;
        case (EntityEnum::kAssignEntity):
            return DesignEntity::kAssign;
        case (EntityEnum::kCallEntity):
            return DesignEntity::kCall;
        case (EntityEnum::kPrintEntity):
            return DesignEntity::kPrint;
        case (EntityEnum::kReadEntity):
            return DesignEntity::kRead;
        case (EntityEnum::kProcedureEntity):
            return DesignEntity::kProcedure;
        case (EntityEnum::kVariableEntity):
            return DesignEntity::kVariable;
        case (EntityEnum::kConstantEntity):
            return DesignEntity::kConstant;
        case (EntityEnum::kElseEntity):
        case (EntityEnum::kNone):
            return DesignEntity::kInvalid;
    }
}

Attribute PKB::GetAttributeFromEntity(Entity* entity) {
  EntityEnum entity_enum = entity->GetEntityEnum();
  switch (entity_enum) {
    case (EntityEnum::kIfEntity):
    case (EntityEnum::kWhileEntity):
    case (EntityEnum::kAssignEntity):
    case (EntityEnum::kCallEntity):
    case (EntityEnum::kPrintEntity):
    case (EntityEnum::kReadEntity):
      return Attribute::kStmtNumber;
    case (EntityEnum::kProcedureEntity):
      return Attribute::kProcName;
    case (EntityEnum::kVariableEntity):
      return Attribute::kVarName;
    case (EntityEnum::kConstantEntity):
      return Attribute::kValue;
    case (EntityEnum::kElseEntity):
    case (EntityEnum::kNone):
      return Attribute::kInvalid;
  }
}

std::vector<DesignEntity> PKB::GetApplicableTypes(DesignEntity de) {
  std::vector<DesignEntity> types {de};
  if (stmt_design_entities_.find(de) != stmt_design_entities_.end()) {
    types.push_back(DesignEntity::kStmt);
  }
  return types;
}

// todo: refactor the below

void PKB::PopulateContainerUse(std::unordered_map<Container*, std::list<Variable*>*> container_use_hash_) {
  for (std::pair<Container*, std::list<Variable*>*> kv: container_use_hash_) {
    std::string k_string;

    if (dynamic_cast<Statement*>(kv.first) != nullptr) {
      Statement* stmt = dynamic_cast<Statement*>(kv.first);
      auto* k_number = const_cast<StatementNumber*>(stmt->GetStatementNumber());
      k_string = std::to_string(k_number->GetNum());
    } else if (dynamic_cast<Procedure*>(kv.first) != nullptr) {
      Procedure* proc = dynamic_cast<Procedure*>(kv.first);
      ProcedureName* proc_name = const_cast<ProcedureName*>(proc->GetName());
      k_string = proc_name->getName();
    }
    for (Entity* entity : *kv.second) {
      relationship_set_.insert({PKBRelRefs::kUsesC, k_string, GetNameFromEntity(entity)});
      relationship_table_[PKBRelRefs::kUsesC][k_string].push_back(entity);
      Entity* first_entity = dynamic_cast<Entity*>(kv.first);
      DesignEntity first_type = GetDesignEntityFromEntity(first_entity);
      DesignEntity second_type = GetDesignEntityFromEntity(entity);
      relationship_by_types_table_[PKBRelRefs::kUsesC][{first_type, second_type}].push_back({first_entity, entity});
      first_param_by_types_table_[PKBRelRefs::kUsesC][{first_type, second_type}].push_back(first_entity);
      if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
        relationship_by_types_table_[PKBRelRefs::kUsesC][{DesignEntity::kStmt, second_type}].push_back({first_entity, entity});
        first_param_by_types_table_[PKBRelRefs::kUsesC][{DesignEntity::kStmt, second_type}].push_back(first_entity);
      }
    }
  }
}

void PKB::PopulateContainerUsedBy(std::unordered_map<Variable*, std::list<Container*>*> container_used_by_hash_) {
  for (std::pair<Variable*, std::list<Container*>*> kv: container_used_by_hash_) {
    std::string k_string;

    Variable* var = dynamic_cast<Variable*>(kv.first);
    VariableName* variable_name = const_cast<VariableName*>(var->GetName());
    k_string = variable_name->getName();

    for (Container* container : *kv.second) {
      Entity* entity = dynamic_cast<Entity*>(container);
      relationship_set_.insert({PKBRelRefs::kUsedByC, k_string, GetNameFromEntity(entity)});
      relationship_table_[PKBRelRefs::kUsedByC][k_string].push_back(entity);
      Entity* first_entity = dynamic_cast<Entity*>(kv.first);
      DesignEntity first_type = GetDesignEntityFromEntity(first_entity);
      DesignEntity second_type = GetDesignEntityFromEntity(entity);
      relationship_by_types_table_[PKBRelRefs::kUsedByC][{first_type, second_type}].push_back({first_entity, entity});
      first_param_by_types_table_[PKBRelRefs::kUsedByC][{first_type, second_type}].push_back(first_entity);
      if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
        relationship_by_types_table_[PKBRelRefs::kUsedByC][{first_type, DesignEntity::kStmt}].push_back({first_entity, entity});
        first_param_by_types_table_[PKBRelRefs::kUsedByC][{first_type, DesignEntity::kStmt}].push_back(first_entity);
      }
    }
  }
}

void PKB::PopulateContainerModifies(std::unordered_map<Container*, std::list<Variable*>*> container_modifies_hash_) {
  for (std::pair<Container*, std::list<Variable*>*> kv: container_modifies_hash_) {
    std::string k_string;

    if (dynamic_cast<Statement*>(kv.first) != nullptr) {
      Statement* stmt = dynamic_cast<Statement*>(kv.first);
      auto* k_number = const_cast<StatementNumber*>(stmt->GetStatementNumber());
      k_string = std::to_string(k_number->GetNum());
    } else if (dynamic_cast<Procedure*>(kv.first) != nullptr) {
      Procedure* proc = dynamic_cast<Procedure*>(kv.first);
      ProcedureName* proc_name = const_cast<ProcedureName*>(proc->GetName());
      k_string = proc_name->getName();
    }
    for (Entity* entity : *kv.second) {
      relationship_set_.insert({PKBRelRefs::kModifiesContainer, k_string, GetNameFromEntity(entity)});
      relationship_table_[PKBRelRefs::kModifiesContainer][k_string].push_back(entity);
      Entity* first_entity = dynamic_cast<Entity*>(kv.first);
      DesignEntity first_type = GetDesignEntityFromEntity(first_entity);
      DesignEntity second_type = GetDesignEntityFromEntity(entity);
      relationship_by_types_table_[PKBRelRefs::kModifiesContainer][{first_type, second_type}].push_back({first_entity, entity});
      first_param_by_types_table_[PKBRelRefs::kModifiesContainer][{first_type, second_type}].push_back(first_entity);
      if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
        relationship_by_types_table_[PKBRelRefs::kModifiesContainer][{DesignEntity::kStmt, second_type}].push_back({first_entity, entity});
        first_param_by_types_table_[PKBRelRefs::kModifiesContainer][{DesignEntity::kStmt, second_type}].push_back(first_entity);
      }
    }
  }
}

void PKB::PopulateContainerModifiedBy(std::unordered_map<Variable*, std::list<Container*>*> container_used_by_hash_) {
  for (std::pair<Variable*, std::list<Container*>*> kv: container_used_by_hash_) {
    std::string k_string;

    Variable* var = dynamic_cast<Variable*>(kv.first);
    VariableName* variable_name = const_cast<VariableName*>(var->GetName());
    k_string = variable_name->getName();

    for (Container* container : *kv.second) {
      Entity* entity = dynamic_cast<Entity*>(container);
      relationship_set_.insert({PKBRelRefs::kModifiedByContainer, k_string, GetNameFromEntity(entity)});
      relationship_table_[PKBRelRefs::kModifiedByContainer][k_string].push_back(entity);
      Entity* first_entity = dynamic_cast<Entity*>(kv.first);
      DesignEntity first_type = GetDesignEntityFromEntity(first_entity);
      DesignEntity second_type = GetDesignEntityFromEntity(entity);
      relationship_by_types_table_[PKBRelRefs::kModifiedByContainer][{first_type, second_type}].push_back({first_entity, entity});
      first_param_by_types_table_[PKBRelRefs::kModifiedByContainer][{first_type, second_type}].push_back(first_entity);
      if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
        relationship_by_types_table_[PKBRelRefs::kModifiedByContainer][{first_type, DesignEntity::kStmt}].push_back({first_entity, entity});
        first_param_by_types_table_[PKBRelRefs::kModifiedByContainer][{first_type, DesignEntity::kStmt}].push_back(first_entity);
      }
    }
  }
}

void PKB::PopulateUses() {
  std::unordered_map<std::string, std::vector<Entity*>> usesS = relationship_table_[PKBRelRefs::kUsesS];
  std::unordered_map<std::string, std::vector<Entity*>> usesC = relationship_table_[PKBRelRefs::kUsesC];
  for (auto pair : usesS) {
    relationship_table_[PKBRelRefs::kUses].insert(pair);
    for (auto second_param : pair.second) {
      relationship_set_.insert({PKBRelRefs::kUses, pair.first, GetNameFromEntity(second_param)});
    }
  }
  for (auto pair : usesC) {
    if (isdigit(pair.first[0])) {
      relationship_table_[PKBRelRefs::kUses].insert(pair);
      for (auto second_param : pair.second) {
        relationship_set_.insert({PKBRelRefs::kUses, pair.first, GetNameFromEntity(second_param)});
      }
    }
  }

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  s_combo_map = relationship_by_types_table_[PKBRelRefs::kUsesS];

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  c_combo_map = relationship_by_types_table_[PKBRelRefs::kUsesC];

  for (auto pair : s_combo_map) {
    relationship_by_types_table_[PKBRelRefs::kUses][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kUses][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );

    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kUses][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }

  for (auto pair : c_combo_map) {
    if (std::get<0>(pair.first) == DesignEntity::kProcedure) {
      continue;
    }
    relationship_by_types_table_[PKBRelRefs::kUses][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kUses][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );

    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kUses][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }
}

void PKB::PopulateModifies() {
  std::unordered_map<std::string, std::vector<Entity*>> modifiesS = relationship_table_[PKBRelRefs::kModifiesStatement];
  std::unordered_map<std::string, std::vector<Entity*>> modifiesC = relationship_table_[PKBRelRefs::kModifiesContainer];
  for (auto pair : modifiesS) {
    relationship_table_[PKBRelRefs::kModifies].insert(pair);
    for (auto second_param : pair.second) {
      relationship_set_.insert({PKBRelRefs::kModifies, pair.first, GetNameFromEntity(second_param)});
    }
  }
  for (auto pair : modifiesC) {
    if (isdigit(pair.first[0])) {
      relationship_table_[PKBRelRefs::kModifies].insert(pair);
      for (auto second_param : pair.second) {
        relationship_set_.insert({PKBRelRefs::kModifies, pair.first, GetNameFromEntity(second_param)});
      }
    }
  }

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  s_combo_map = relationship_by_types_table_[PKBRelRefs::kModifiesStatement];

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  c_combo_map = relationship_by_types_table_[PKBRelRefs::kModifiesContainer];

  for (auto pair : s_combo_map) {
    relationship_by_types_table_[PKBRelRefs::kModifies][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kModifies][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );
    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kModifies][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }

  for (auto pair : c_combo_map) {
    if (std::get<0>(pair.first) == DesignEntity::kProcedure) {
      continue;
    }
    relationship_by_types_table_[PKBRelRefs::kModifies][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kModifies][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );
    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kModifies][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }
}

void PKB::PopulateUsedBy() {
  std::unordered_map<std::string, std::vector<Entity*>> usedByS = relationship_table_[PKBRelRefs::kUsedByS];
  std::unordered_map<std::string, std::vector<Entity*>> usedByC = relationship_table_[PKBRelRefs::kUsedByC];
  for (auto pair : usedByS) {
    relationship_table_[PKBRelRefs::kUsedBy].insert(pair);
    for (auto second_param : pair.second) {
      relationship_set_.insert({PKBRelRefs::kUsedBy, pair.first, GetNameFromEntity(second_param)});
    }
  }
  for (auto pair : usedByC) {
    for (auto second_param : pair.second) {
      if (dynamic_cast<Procedure*>(second_param) != nullptr) {
        continue;
      }
      relationship_table_[PKBRelRefs::kUsedBy][pair.first].push_back(second_param);
      relationship_set_.insert({PKBRelRefs::kUsedBy, pair.first, GetNameFromEntity(second_param)});
    }
  }

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  s_combo_map = relationship_by_types_table_[PKBRelRefs::kUsedByS];

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  c_combo_map = relationship_by_types_table_[PKBRelRefs::kUsedByC];

  for (auto pair : s_combo_map) {
    relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );
    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }

  for (auto pair : c_combo_map) {
    if (std::get<1>(pair.first) == DesignEntity::kProcedure) {
      continue;
    }
    relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );
    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }
}

void PKB::PopulateModifiedBy() {
  std::unordered_map<std::string, std::vector<Entity*>> modifiedByS = relationship_table_[PKBRelRefs::kModifiedByStatement];
  std::unordered_map<std::string, std::vector<Entity*>> modifiedByC = relationship_table_[PKBRelRefs::kModifiedByContainer];
  for (auto pair : modifiedByS) {
    relationship_table_[PKBRelRefs::kModifiedBy].insert(pair);
    for (auto second_param : pair.second) {
      relationship_set_.insert({PKBRelRefs::kModifiedBy, pair.first, GetNameFromEntity(second_param)});
    }
  }
  for (auto pair : modifiedByC) {
    for (auto second_param : pair.second) {
      if (dynamic_cast<Procedure*>(second_param) != nullptr) {
        continue;
      }
      relationship_table_[PKBRelRefs::kModifiedBy][pair.first].push_back(second_param);
      relationship_set_.insert({PKBRelRefs::kModifiedBy, pair.first, GetNameFromEntity(second_param)});
    }
  }

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  s_combo_map = relationship_by_types_table_[PKBRelRefs::kModifiedByStatement];

  std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
  c_combo_map = relationship_by_types_table_[PKBRelRefs::kModifiedByContainer];

  for (auto pair : s_combo_map) {
    relationship_by_types_table_[PKBRelRefs::kModifiedBy][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kModifiedBy][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );
    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }

  for (auto pair : c_combo_map) {
    if (std::get<1>(pair.first) == DesignEntity::kProcedure) {
      continue;
    }
    relationship_by_types_table_[PKBRelRefs::kModifiedBy][pair.first].insert(
      std::end(relationship_by_types_table_[PKBRelRefs::kModifiedBy][pair.first]),
      std::begin(pair.second),
      std::end(pair.second)
      );
    for (auto entity_pairs : pair.second) {
      first_param_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
    }
  }
}