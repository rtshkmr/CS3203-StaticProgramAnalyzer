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

  std::vector<DesignEntity> design_entities {
    DesignEntity::kProcedure,
    DesignEntity::kVariable,
    DesignEntity::kConstant,
    DesignEntity::kCall,
    DesignEntity::kPrint,
    DesignEntity::kRead,
    DesignEntity::kIf,
    DesignEntity::kWhile,
    DesignEntity::kAssign,
    DesignEntity::kStmt,
  };

  PopulateEntities(DesignEntity::kProcedure, d.proc_list_);
  PopulateEntities(DesignEntity::kVariable, d.var_list_);
  PopulateEntities(DesignEntity::kConstant, d.const_list_);
  PopulateEntities(DesignEntity::kCall, d.call_list_);
  PopulateEntities(DesignEntity::kPrint, d.print_list_);
  PopulateEntities(DesignEntity::kRead, d.read_list_);
  PopulateEntities(DesignEntity::kIf, d.if_list_);
  PopulateEntities(DesignEntity::kWhile, d.while_list_);
  PopulateEntities(DesignEntity::kAssign, d.assign_list_);
  PopulateEntities(DesignEntity::kStmt, d.stmt_list_);
  
  PopulateRelationship(&d.follow_hash_, PKBRelRefs::kFollows);
  PopulateRelationship(&d.followed_by_hash_, PKBRelRefs::kFollowedBy);
  PopulateRelationship(&d.follows_T_hash_, PKBRelRefs::kFollowsT);
  PopulateRelationship(&d.followed_by_T_hash_, PKBRelRefs::kFollowedByT);
  PopulateRelationship(&d.parent_to_child_hash_, PKBRelRefs::kParent);
  PopulateRelationship(&d.parent_to_child_T_hash_, PKBRelRefs::kParentT);
  PopulateRelationship(&d.child_to_parent_hash_, PKBRelRefs::kChild);
  PopulateRelationship(&d.child_to_parent_T_hash_, PKBRelRefs::kChildT);
  PopulateRelationship(&d.calls_hash_, PKBRelRefs::kCalls);
  PopulateRelationship(&d.called_by_hash_, PKBRelRefs::kCalledBy);
  PopulateRelationship(&d.calls_T_hash_, PKBRelRefs::kCallsT);
  PopulateRelationship(&d.called_by_T_hash_, PKBRelRefs::kCalledByT);
  PopulateRelationship(&d.next_hash_, PKBRelRefs::kNext);
  PopulateRelationship(&d.previous_hash_, PKBRelRefs::kPrevious);

  PopulateRelationship(&d.use_hash_, PKBRelRefs::kUses);
  PopulateRelationship(&d.used_by_hash_, PKBRelRefs::kUsedBy);
  PopulateRelationship(&d.modifies_hash_, PKBRelRefs::kModifies);
  PopulateRelationship(&d.modified_by_hash_, PKBRelRefs::kModifiedBy);
  PopulateRelationship(&d.container_use_hash_, PKBRelRefs::kUses);
  PopulateRelationship(&d.container_used_by_hash_, PKBRelRefs::kUsedBy);
  PopulateRelationship(&d.container_modifies_hash_, PKBRelRefs::kModifies);
  PopulateRelationship(&d.container_modified_by_hash_, PKBRelRefs::kModifiedBy);


  ProcessEntitiesWithMatchingAttributes();

  L("[DONE] PKB POPULATED WITHOUT ERROR\n");
}

template <typename T>
void PKB::PopulateEntities(DesignEntity design_entity, T& entity_list) {
  for (auto entity : entity_list) {
    type_to_entity_map_[design_entity].push_back(entity);
    std::string entity_name = GetNameFromEntity(entity);
    std::unordered_map<Attribute, std::string> attributes = GetAttributesFromEntity(entity);
    for (std::pair<Attribute, std::string> attribute: attributes) {
      attribute_to_entity_map_[{design_entity, attribute.first}][attribute.second].push_back(entity);
      attribute_string_to_entity_map_[attribute.second].insert(entity);
    }
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
    Entity* first_entity = dynamic_cast<Entity*>(kv.first);
    std::string k_string = GetNameFromEntity(first_entity);
    DesignEntity first_type = GetDesignEntityFromEntity(first_entity);
    std::vector<DesignEntity> first_types = GetApplicableTypes(first_type);
    std::vector<DesignEntity> second_types;

    for (Y* e : *kv.second) {
      Entity* entity = dynamic_cast<Entity*>(e);
      relationship_set_.insert({ref, k_string, GetNameFromEntity(entity)});
      relationship_table_[ref][k_string].push_back(entity);

      DesignEntity second_type = GetDesignEntityFromEntity(entity);
      second_types = GetApplicableTypes(second_type);

      for (DesignEntity type1 : first_types) {
        for (DesignEntity type2 : second_types) {
          relationship_by_types_table_[ref][{type1, type2}].push_back({first_entity, entity});
          first_param_by_types_table_[ref][{type1, type2}].push_back(first_entity);
        }
      }
    }
  }
}

std::vector<Entity*> PKB::GetRelationship(PKBRelRefs ref, std::string entity) {
    return relationship_table_[ref][entity];
}

std::vector<std::tuple<Entity*, Entity*>> PKB::GetRelationshipByTypes(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
    if (d1 == DesignEntity::kProgLine) d1 = DesignEntity::kStmt;
    if (d2 == DesignEntity::kProgLine) d2 = DesignEntity::kStmt;
    return relationship_by_types_table_[ref][{d1, d2}];
}

std::vector<Entity*> PKB::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
  if (d1 == DesignEntity::kProgLine) d1 = DesignEntity::kStmt;
  if (d2 == DesignEntity::kProgLine) d2 = DesignEntity::kStmt;

  return first_param_by_types_table_[ref][{d1, d2}];
}

std::vector<Entity*> PKB::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity d) {
    if (d == DesignEntity::kProgLine) d = DesignEntity::kStmt;
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
    if (de == DesignEntity::kProgLine) de = DesignEntity::kStmt;
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
  return !relationship_table_[ref][e].empty();
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


std::string PKB::GetNameFromEntity(Entity* entity) {
    EntityEnum e = entity->GetEntityEnum();
    if (e == EntityEnum::kProcedureEntity) {
        Procedure* proc = dynamic_cast<Procedure*>(entity);
        ProcedureName* proc_name = const_cast<ProcedureName*>(proc->GetName());
        return proc_name->getName();
    } else if (e == EntityEnum::kVariableEntity) {
        Variable* var = dynamic_cast<Variable*>(entity);
        VariableName* variable_name = const_cast<VariableName*>(var->GetName());
        return variable_name->getName();
    } else if (e == EntityEnum::kConstantEntity) {
      Constant* constant = dynamic_cast<Constant*>(entity);
      ConstantValue* cv = const_cast<ConstantValue*>(constant->GetValue());
      return std::to_string(cv->Get());
    } else if (Statement* stmt = dynamic_cast<Statement*>(entity)) {
      auto* k_number = const_cast<StatementNumber*>(stmt->GetStatementNumber());
      return std::to_string(k_number->GetNum());
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

std::vector<Attribute> PKB::GetAttributeTypes(Entity* entity) {
  EntityEnum entity_enum = entity->GetEntityEnum();
  switch (entity_enum) {
    case (EntityEnum::kProcedureEntity):
      return std::vector<Attribute> {Attribute::kProcName};
    case (EntityEnum::kVariableEntity):
      return std::vector<Attribute> {Attribute::kVarName};
    case (EntityEnum::kConstantEntity):
      return std::vector<Attribute> {Attribute::kValue};
    case (EntityEnum::kElseEntity):
    case (EntityEnum::kNone): 
      return std::vector<Attribute> {Attribute::kInvalid};
    case (EntityEnum::kIfEntity):
    case (EntityEnum::kWhileEntity):
    case (EntityEnum::kAssignEntity):
      return std::vector<Attribute> {Attribute::kStmtNumber};
    case (EntityEnum::kCallEntity):
      return std::vector<Attribute> {Attribute::kStmtNumber, Attribute::kProcName};
    case (EntityEnum::kPrintEntity):
    case (EntityEnum::kReadEntity):
      return std::vector<Attribute> {Attribute::kStmtNumber, Attribute::kVarName};
  }
}

std::unordered_map<Attribute, std::string> PKB::GetAttributesFromEntity(Entity* entity) {
  std::vector<Attribute> attribute_types = GetAttributeTypes(entity);
  std::unordered_map<Attribute, std::string> attribute_map;
  for (Attribute attribute : attribute_types) {
    attribute_map[attribute] = GetAttributeFromEntity(entity, attribute);
  }
  return attribute_map
}

std::string PKB::GetAttributeFromEntity(Entity* entity, Attribute attribute) {
  EntityEnum entity_enum = entity->GetEntityEnum();

  if (attribute == Attribute::kStmtNumber) {
    return GetNameFromEntity(entity);
  } else if (entity_enum == EntityEnum::kCallEntity) {
    CallEntity* call_entity = dynamic_cast<CallEntity*>(entity);
  } else if (entity_enum == EntityEnum::kPrintEntity) {
    PrintEntity* print_entity = dynamic_cast<PrintEntity*>(entity);
  } else if (entity_enum == EntityEnum::kReadEntity) {
    ReadEntity* read_entity = dynamic_cast<ReadEntity*>(entity);
  } else {
    return GetNameFromEntity(entity);
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
