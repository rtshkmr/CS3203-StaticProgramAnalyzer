#include <exception/SpaException.h>
#include "PKB.h"
#include <stdio.h>
#include <iostream>
#include <util/Logger.h>
#include <util/Utility.h>

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

  std::list<Constant*> constants;
  std::unordered_set<std::string> seen = {};
  for (Constant* constant : *d.GetConstantList()) {
    std::string c_string = GetNameFromEntity(constant);
    if (seen.find(c_string) == seen.end()) {
      constants.push_back(constant);
      seen.insert(c_string);
    }
  }

  // Populate Entities
  PopulateEntities(DesignEntity::kProcedure, *d.GetProcList());
  PopulateEntities(DesignEntity::kVariable, *d.GetVariableList());
  PopulateEntities(DesignEntity::kConstant, constants);
  PopulateEntities(DesignEntity::kCall, *d.GetCallList());
  PopulateEntities(DesignEntity::kPrint, *d.GetPrintList());
  PopulateEntities(DesignEntity::kRead, *d.GetReadList());
  PopulateEntities(DesignEntity::kIf, *d.GetIfList());
  PopulateEntities(DesignEntity::kWhile, *d.GetWhileList());
  PopulateEntities(DesignEntity::kAssign, *d.GetAssignList());
  PopulateEntities(DesignEntity::kStmt, *d.GetStatementList());

  // Populate non-transitive relationships
  PopulateRelationship(d.GetFollowsMap(), PKBRelRefs::kFollows);
  PopulateRelationship(d.GetFollowedByMap(), PKBRelRefs::kFollowedBy);
  PopulateRelationship(d.GetParentMap(), PKBRelRefs::kParent);
  PopulateRelationship(d.GetChildMap(), PKBRelRefs::kChild);
  PopulateRelationship(d.GetUseSMap(), PKBRelRefs::kUses);
  PopulateRelationship(d.GetUsedBySMap(), PKBRelRefs::kUsedBy);
  PopulateRelationship(d.GetModifiesSMap(), PKBRelRefs::kModifies);
  PopulateRelationship(d.GetModifiedBySMap(), PKBRelRefs::kModifiedBy);
  PopulateRelationship(d.GetCallsMap(), PKBRelRefs::kCalls);
  PopulateRelationship(d.GetCalledByMap(), PKBRelRefs::kCalledBy);
  PopulateRelationship(d.GetNextMap(), PKBRelRefs::kNext);
  PopulateRelationship(d.GetPrevMap(), PKBRelRefs::kPrevious);

  // Populate transitive relationships
  PopulateRelationship(d.GetFollowsTMap(), PKBRelRefs::kFollowsT);
  PopulateRelationship(d.GetFollowedByTMap(), PKBRelRefs::kFollowedByT);
  PopulateRelationship(d.GetParentTMap(), PKBRelRefs::kParentT);
  PopulateRelationship(d.GetChildTMap(), PKBRelRefs::kChildT);
  PopulateRelationship(d.GetCallsTMap(), PKBRelRefs::kCallsT);
  PopulateRelationship(d.GetCalledByTMap(), PKBRelRefs::kCalledByT);

  // Populate container relationships
  PopulateRelationship(d.GetUseCMap(), PKBRelRefs::kUses);
  PopulateRelationship(d.GetUsedByCMap(), PKBRelRefs::kUsedBy);
  PopulateRelationship(d.GetModifiesCMap(), PKBRelRefs::kModifies);
  PopulateRelationship(d.GetModifiedByCMap(), PKBRelRefs::kModifiedBy);


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
      entity_to_attribute_type_map_[entity][attribute.second] = attribute.first;
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
          pattern_variables.push_back(assign_entity->GetVariableObj());
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


std::unordered_map<std::string, std::vector<Entity*>> PKB::GetRelationshipMap(PKBRelRefs ref) {
  return relationship_table_[ref];
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
    } else {
      return std::vector<Entity*>{};
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

std::vector<entity_pair> PKB::GetEntitiesWithMatchingAttributes(type_attribute_pair type_one, type_attribute_pair type_two) {
  auto hi = entities_with_matching_attributes_map_[type_one][type_two];
  return entities_with_matching_attributes_map_[type_one][type_two];
}

bool PKB::HasRelationship(PKBRelRefs ref) {
  return !relationship_table_[ref].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
  return !relationship_by_types_table_[ref][{d1, d2}].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, std::string e) {
  return !relationship_table_[ref][e].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, std::string e1, std::string e2) {
  return relationship_set_.find({ref, e1, e2}) != relationship_set_.end();
}

void PKB::ProcessEntitiesWithMatchingAttributes() {
  for (auto kv : attribute_string_to_entity_map_) {
    std::vector<Entity*> entities(kv.second.begin(), kv.second.end());
    for (int i = 0; i < entities.size(); i++) {
      Entity* entity_one = entities[i];
      DesignEntity design_entity_one = GetDesignEntityFromEntity(entity_one);
      auto first_entity_types = GetApplicableTypes(design_entity_one);
      Attribute attribute_one = entity_to_attribute_type_map_[entity_one][kv.first];
      for (int j = i; j < entities.size(); j++) {
        Entity* entity_two = entities[j];
        DesignEntity design_entity_two = GetDesignEntityFromEntity(entity_two);
//<<<<<<< HEAD
        auto second_entity_types = GetApplicableTypes(design_entity_two);
        Attribute attribute_two = entity_to_attribute_type_map_[entity_two][kv.first];
        for (auto first_type : first_entity_types) {
          for (auto second_type : second_entity_types) {
            entities_with_matching_attributes_map_[{first_type, attribute_one}][{second_type, attribute_two}]
              .push_back({entity_one, entity_two});
            entities_with_matching_attributes_map_[{second_type, attribute_two}][{first_type, attribute_one}]
              .push_back({entity_two, entity_one});
          }
        }
//=======
//        entities_with_matching_attributes_map_[{design_entity_one, design_entity_two}]
//            .push_back({entity_one, entity_two});
//>>>>>>> master
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
    VariableName* variable_name = const_cast<VariableName*>(var->GetVariableName());
    return variable_name->GetName();
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
  for (Attribute attribute : attribute_types)
    attribute_map[attribute] = GetAttributeFromEntity(entity, attribute);
  return attribute_map;
}

std::string PKB::GetAttributeFromEntity(Entity* entity, Attribute attribute) {
  EntityEnum entity_enum = entity->GetEntityEnum();

  if (attribute == Attribute::kStmtNumber) {
    return GetNameFromEntity(entity);
  } else if (entity_enum == EntityEnum::kCallEntity) {
    CallEntity* call_entity = dynamic_cast<CallEntity*>(entity);
    return GetNameFromEntity(call_entity->GetProcedure());
  } else if (entity_enum == EntityEnum::kPrintEntity) {
    PrintEntity* print_entity = dynamic_cast<PrintEntity*>(entity);
    return GetNameFromEntity(print_entity->GetVariable());
  } else if (entity_enum == EntityEnum::kReadEntity) {
    ReadEntity* read_entity = dynamic_cast<ReadEntity*>(entity);
    return GetNameFromEntity(read_entity->GetVariable());
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

std::unordered_map<Entity*,
                   std::list<Entity*>*>* PKB::ConvertStringToEntityMapping(const std::unordered_map<std::string,
                                                                                                    std::vector<Entity*>> &pkb_map) {
  auto* entity_map = new std::unordered_map<Entity*, std::list<Entity*>*>{};
  for (auto[key, value] : pkb_map) {
    Entity* first_arg = type_to_entity_map_[DesignEntity::kStmt][Utility::ConvertStringToInt(key) - 1];
    auto* second_arg = new std::list<Entity*>{};
    second_arg->insert(second_arg->end(), value.begin(), value.end());
    entity_map->insert({first_arg, second_arg});
  }
  return entity_map;
}
