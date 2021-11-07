#include <exception/SpaException.h>
#include "PKB.h"
#include <iostream>
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

  program_ = d.GetProgram();

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
    auto pattern_entities = GetPatternEntitySet();
    if (pattern_entities.find(design_entity) != pattern_entities.end()) {
      pattern_maps_[design_entity][entity_name].push_back(entity);
      auto* casted_entity = reinterpret_cast<Entity*>(entity);
      PopulatePatternVariables(casted_entity);
    }
  }
}

void PKB::PopulatePatternVariables(Entity* entity) {
  std::vector<Variable*> pattern_variables;
  DesignEntity design_entity = GetDesignEntityFromEntity(entity);
  EntityEnum entity_enum = entity->GetEntityEnum();
  switch (entity_enum) {
    case EntityEnum::kWhileEntity: {
      auto* while_entity = reinterpret_cast<WhileEntity*>(entity);
      pattern_variables = while_entity->GetControlVariables();
    } break;
    case EntityEnum::kIfEntity: {
      auto* if_entity = reinterpret_cast<IfEntity*>(entity);
      pattern_variables = if_entity->GetControlVariables();
    } break;
    case EntityEnum::kAssignEntity: {
      auto* assign_entity = reinterpret_cast<AssignEntity*>(entity);
      pattern_variables.push_back(assign_entity->GetVariableObj());
    } break;
    default: {
      throw PKBException("Invalid EntityEnum in pattern entity.");
    }
  }

  for (Variable* variable : pattern_variables) {
    pattern_maps_[design_entity][GetNameFromEntity(variable)].push_back(entity);
  }
}


std::unordered_map<std::string, std::vector<Entity*>> PKB::GetRelationshipMap(PKBRelRefs ref) {
  return relationship_table_[ref];
}

std::vector<Entity*> PKB::GetRelationship(PKBRelRefs ref, const std::string& entity) {
  return relationship_table_[ref][entity];
}

std::vector<entity_pair> PKB::GetRelationshipByFirst(PKBRelRefs ref, const std::string& entity, const type_combo& t_c) {
  return relationship_by_first_entity_table_[ref][entity][t_c];
}

std::vector<entity_pair> PKB::GetRelationshipBySecond(PKBRelRefs ref, const std::string& entity, const type_combo& t_c) {
  return relationship_by_second_entity_table_[ref][entity][t_c];
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

std::vector<Entity*> PKB::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity design_entity) {
  if (design_entity == DesignEntity::kProgLine) design_entity = DesignEntity::kStmt;
  auto second_param_valid_statement_set = GetSecondParamValidStatementSet();
  if (second_param_valid_statement_set.find(ref) != second_param_valid_statement_set.end()) {
    return first_param_by_types_table_[ref][{design_entity, DesignEntity::kStmt}];
  } else {
    auto second_param_is_var = GetSecondParamVarSet();
    auto second_param_is_proc = GetSecondParamProcSet();
    if (second_param_is_var.find(ref) != second_param_is_var.end()) {
      return first_param_by_types_table_[ref][{design_entity, DesignEntity::kVariable}];
    } else if (second_param_is_proc.find(ref) != second_param_is_proc.end()) {
      return first_param_by_types_table_[ref][{design_entity, DesignEntity::kProcedure}];
    } else {
      return std::vector<Entity*>{};
    }
  }
}

std::vector<Entity*> PKB::GetDesignEntities(DesignEntity de) {
  if (de == DesignEntity::kProgLine) de = DesignEntity::kStmt;
  return type_to_entity_map_[de];
}

std::vector<Entity*> PKB::GetPatternEntities(DesignEntity de, const std::string& var_or_stmt) {
  return pattern_maps_[de][var_or_stmt];
}

std::vector<Entity*> PKB::GetEntitiesWithAttributeValue(DesignEntity design_entity, Attribute attribute, const std::string& value) {
  return attribute_to_entity_map_[{design_entity, attribute}][value];
}

std::vector<entity_pair> PKB::GetEntitiesWithMatchingAttributes(type_attribute_pair type_one, type_attribute_pair type_two) {
  if (std::get<0>(type_one) == DesignEntity::kProgLine)
    type_one = {DesignEntity::kStmt, Attribute::kStmtNumber};
  if (std::get<0>(type_two) == DesignEntity::kProgLine)
    type_two = {DesignEntity::kStmt, Attribute::kStmtNumber};
  return entities_with_matching_attributes_map_[type_one][type_two];
}

bool PKB::HasRelationship(PKBRelRefs ref) {
  return !relationship_table_[ref].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
  return !relationship_by_types_table_[ref][{d1, d2}].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, const std::string& e) {
  return !relationship_table_[ref][e].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, const std::string& e1, const std::string& e2) {
  return relationship_set_.find({ref, e1, e2}) != relationship_set_.end();
}

void PKB::ProcessEntitiesWithMatchingAttributes() {
  for (auto kv : attribute_string_to_entity_map_) {
    std::vector<Entity*> entities(kv.second.begin(), kv.second.end());
    std::string attribute_string = kv.first;
    PopulateMatchingEntities(entities, attribute_string);
  }
}
void PKB::PopulateMatchingEntities(std::vector<Entity*>& entities, std::string& attribute_string) {
  for (int i = 0; i < entities.size(); i++) {
    Entity* entity_one = entities[i];
    DesignEntity design_entity_one = GetDesignEntityFromEntity(entity_one);
    auto first_entity_types = GetApplicableTypes(design_entity_one);
    Attribute attribute_one = entity_to_attribute_type_map_[entity_one][attribute_string];
    for (int j = i; j < entities.size(); j++) {
      Entity* entity_two = entities[j];
      DesignEntity design_entity_two = GetDesignEntityFromEntity(entity_two);
      auto second_entity_types = GetApplicableTypes(design_entity_two);
      Attribute attribute_two = entity_to_attribute_type_map_[entity_two][attribute_string];
      PopulateMatchingEntities(entity_one,
                               entity_two,
                               attribute_one,
                               attribute_two,
                               first_entity_types,
                               second_entity_types);
    }
  }
}
void PKB::PopulateMatchingEntities(Entity* entity_one,
                                   Entity* entity_two,
                                   Attribute& attribute_one,
                                   Attribute& attribute_two,
                                   std::vector<DesignEntity>& first_entity_types,
                                   std::vector<DesignEntity>& second_entity_types) {
  for (auto first_type : first_entity_types) {
    for (auto second_type : second_entity_types) {
      entities_with_matching_attributes_map_[{first_type, attribute_one}][{second_type, attribute_two}]
      .push_back({entity_one, entity_two});
      entities_with_matching_attributes_map_[{second_type, attribute_two}][{first_type, attribute_one}]
      .push_back({entity_two, entity_one});
    }
  }
}


std::string PKB::GetNameFromEntity(Entity* entity) {
  EntityEnum e = entity->GetEntityEnum();
  if (e == EntityEnum::kProcedureEntity) {
    auto* proc = dynamic_cast<Procedure*>(entity);
    auto* proc_name = const_cast<ProcedureName*>(proc->GetName());
    return proc_name->GetName();
  } else if (e == EntityEnum::kVariableEntity) {
    auto* var = dynamic_cast<Variable*>(entity);
    auto* variable_name = const_cast<VariableName*>(var->GetVariableName());
    return variable_name->GetName();
  } else if (e == EntityEnum::kConstantEntity) {
    auto* constant = dynamic_cast<Constant*>(entity);
    auto* cv = const_cast<ConstantValue*>(constant->GetValue());
    return std::to_string(cv->GetValue());
  } else if (auto* stmt = dynamic_cast<Statement*>(entity)) {
    auto* k_number = const_cast<StatementNumber*>(stmt->GetStatementNumberObj());
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
    default:
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
    default:
      return std::vector<Attribute> {};
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
    auto* call_entity = dynamic_cast<CallEntity*>(entity);
    return GetNameFromEntity(call_entity->GetCalledProcedure());
  } else if (entity_enum == EntityEnum::kPrintEntity) {
    auto* print_entity = dynamic_cast<PrintEntity*>(entity);
    return GetNameFromEntity(print_entity->GetVariableObj());
  } else if (entity_enum == EntityEnum::kReadEntity) {
    auto* read_entity = dynamic_cast<ReadEntity*>(entity);
    return GetNameFromEntity(read_entity->GetVariableObj());
  } else {
    return GetNameFromEntity(entity);
  }
}

std::vector<DesignEntity> PKB::GetApplicableTypes(DesignEntity de) {
  std::vector<DesignEntity> types {de};
  auto stmt_design_entities = GetStmtDesignEntitySet();
  if (stmt_design_entities.find(de) != stmt_design_entities.end()) {
    types.push_back(DesignEntity::kStmt);
    types.push_back(DesignEntity::kProgLine);
  }
  return types;
}

Program* PKB::GetProgram() {
  return program_;
}
