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

  InitializeDataStructures();

  PopulateProcEntities(d.proc_list_);
  PopulateVarEntities(d.var_list_);
  PopulateConstEntities(d.const_list_);
  PopulateStmtEntities(d.stmt_list_);
  PopulateIfEntities(d.if_list_);
  PopulateWhileEntities(d.while_list_);
  PopulateAssignEntities(d.assign_list_);
  PopulateCallEntities(d.call_list_);
  PopulatePrintEntities(d.print_list_);
  PopulateReadEntities(d.read_list_);

  PopulateFollows(d.follow_hash_);
  PopulateFollowedBy(d.followed_by_hash_);
  PopulateChild(d.child_to_parent_hash_);

  const std::vector<PKBRelRefs> non_proc_refs_to_populate {
      PKBRelRefs::kFollowsT,
      PKBRelRefs::kFollowedByT,
      PKBRelRefs::kParent,
      PKBRelRefs::kParentT,
      PKBRelRefs::kChildT,
      PKBRelRefs::kUsesS,
      PKBRelRefs::kUsesC,
      PKBRelRefs::kUsedByS,
      PKBRelRefs::kUsedByC,
      PKBRelRefs::kModifiesStatement,
      PKBRelRefs::kModifiedByStatement,
  };

  std::vector<std::unordered_map<Entity*, std::list<Entity*>*>*> non_proc_hashes;
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.follows_T_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.followed_by_T_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.parent_to_child_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.parent_to_child_T_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.child_to_parent_T_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.use_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.used_by_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.modifies_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.modified_by_hash_));

  for (int i = 0; i < non_proc_hashes.size(); i++) {
      PopulateRelationship(non_proc_hashes.at(i), non_proc_refs_to_populate.at(i));
  }

  PopulateContainerUse(d.container_use_hash_);
  PopulateContainerUsedBy(d.container_used_by_hash_);
  PopulateContainerModifies(d.container_modifies_hash_);
  PopulateContainerModifiedBy(d.container_modified_by_hash_);

  // TO BE DEPRECATED FROM HERE
//  PopulateProcList(d.proc_list_);
//  PopulateVarList(d.var_list_);
//  PopulateConstList(d.const_list_);
//  PopulateIfList(d.if_list_);
//  PopulateWhileList(d.while_list_);
//  PopulateAssignList(d.assign_list_);
//  PopulateCallList(d.call_list_);
//  PopulatePrintList(d.print_list_);
//  PopulateReadList(d.read_list_);
//  PopulateStmtList(d.stmt_list_);
//
//  PopulateFollowsMap(d.follow_hash_);
//  PopulatePreviousMap(d.followed_by_hash_);
//
//  PopulateParentMap(d.parent_to_child_hash_);
//  PopulateChildMap(d.child_to_parent_hash_);
//
//  PopulateFollowsTMap(d.follows_T_hash_);
//  PopulatePreviousTMap(d.followed_by_T_hash_);
//  PopulateParentTMap(d.parent_to_child_T_hash_);
//  PopulateChildTMap(d.child_to_parent_T_hash_);
//
//  PopulateUseSMap(d.use_hash_);
//  PopulateUsedBySMap(d.used_by_hash_);
//  PopulateUseCMap(d.container_use_hash_);
//  PopulateUsedByCMap(d.container_used_by_hash_);
//  PopulateModifiesSMap(d.modifies_hash_);
//  PopulateModifiedBySMap(d.modified_by_hash_);
//  PopulateModifiesCMap(d.container_modifies_hash_);
//  PopulateModifiedByCMap(d.container_modified_by_hash_);
  // UNTIL HERE

  L("[DONE] PKB POPULATED WITHOUT ERROR\n");
}

std::vector<Entity*> PKB::GetRelationship(PKBRelRefs ref, std::string entity) {
    return relationship_table_[ref][entity];
}

std::vector<std::tuple<Entity*, Entity*>> PKB::GetRelationshipByTypes(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
    return relationship_by_type_table_[ref][{d1, d2}];
}

std::vector<Entity*> PKB::GetRelationshipByType(PKBRelRefs ref, DesignEntity d) {
//    todo: optimize this
    std::vector<std::tuple<DesignEntity, DesignEntity>> combos = first_param_map_[d];
    std::vector<Entity*> entities;
    for (auto combo : combos) {
        std::vector<std::tuple<Entity*, Entity*>> pairs = relationship_by_type_table_[ref][{std::get<0>(combo), std::get<0>(combo)}];
        for (auto pair : pairs) {
            entities.push_back(std::get<0>(pair));
        }
    }
    return entities;
}

void PKB::InitializeDataStructures() {
    for (DesignEntity param1 : all_design_entities) {
        for (DesignEntity param2 : all_design_entities) {
            first_param_map_[param1].push_back(std::make_tuple(param1, param2));
            second_param_map_[param2].push_back(std::make_tuple(param1, param2));
        }
    }

    stmt_design_entities_.insert(DesignEntity::kRead);
    stmt_design_entities_.insert(DesignEntity::kPrint);
    stmt_design_entities_.insert(DesignEntity::kCall);
    stmt_design_entities_.insert(DesignEntity::kWhile);
    stmt_design_entities_.insert(DesignEntity::kIf);
    stmt_design_entities_.insert(DesignEntity::kAssign);
}

void PKB::PopulateProcEntities(const std::list<Procedure*>& proc_list) {
    for (Procedure* proc : proc_list) {
        type_to_entity_map_[DesignEntity::kProcedure].push_back(proc);
        ProcedureName proc_name = *proc->GetName();
        std::string name_string = proc_name.getName();
        entity_string_to_type_map_[name_string] = DesignEntity::kProcedure;
        entity_object_to_type_map_[proc] = DesignEntity::kProcedure;
    }
}

void PKB::PopulateVarEntities(const std::list<Variable*>& var_list) {
    for (Variable* var : var_list) {
        type_to_entity_map_[DesignEntity::kVariable].push_back(var);
        VariableName var_name = *var->GetName();
        std::string name_string = var_name.getName();
        entity_string_to_type_map_[name_string] = DesignEntity::kVariable;
        entity_object_to_type_map_[var] = DesignEntity::kVariable;
    }
}

void PKB::PopulateConstEntities(const std::list<ConstantValue*>& const_list) {
    for (ConstantValue* cv : const_list) {
        Constant* constant = new Constant(cv);
        type_to_entity_map_[DesignEntity::kConstant].push_back(constant);
        int cv_int = cv->Get();
        std::string name_string = std::to_string(cv_int);
        entity_string_to_type_map_["c" + name_string] = DesignEntity::kConstant;
        entity_object_to_type_map_[constant] = DesignEntity::kConstant;
    }
}

void PKB::PopulateStmtEntities(const std::list<Statement*>& stmt_list) {
    for (Statement* stmt : stmt_list) {
        type_to_entity_map_[DesignEntity::kStmt].push_back(stmt);
        StatementNumber stmt_number = *stmt->GetStatementNumber();
        std::string name_string = std::to_string(stmt_number.GetNum());
    }
}

void PKB::PopulateIfEntities(const std::list<IfEntity*>& if_list) {
    for (IfEntity* ifs : if_list) {
        type_to_entity_map_[DesignEntity::kIf].push_back(ifs);
        StatementNumber stmt_number = *ifs->GetStatementNumber();
        std::string name_string = std::to_string(stmt_number.GetNum());
        entity_string_to_type_map_[name_string] = DesignEntity::kIf;
        entity_object_to_type_map_[ifs] = DesignEntity::kIf;
        if_map_[name_string].push_back(ifs);
        for (Variable* var : ifs->GetExpressionVariables()) {
            VariableName* variable_name = const_cast<VariableName*>(var->GetName());
            std::string variable_string = variable_name->getName();
            if_map_[variable_string].push_back(ifs);
        }
    }
}

void PKB::PopulateWhileEntities(const std::list<WhileEntity*>& while_list) {
    for (WhileEntity* while_stmt : while_list) {
        type_to_entity_map_[DesignEntity::kWhile].push_back(while_stmt);
        StatementNumber stmt_number = *while_stmt->GetStatementNumber();
        std::string name_string = std::to_string(stmt_number.GetNum());
        entity_string_to_type_map_[name_string] = DesignEntity::kWhile;
        entity_object_to_type_map_[while_stmt] = DesignEntity::kWhile;
        while_map_[name_string].push_back(while_stmt);
        for (Variable* var : while_stmt->GetExpressionVariables()) {
            VariableName* variable_name = const_cast<VariableName*>(var->GetName());
            std::string variable_string = variable_name->getName();
            while_map_[variable_string].push_back(while_stmt);
        }
    }
}

void PKB::PopulateAssignEntities(const std::list<AssignEntity*>& assign_list) {
    for (AssignEntity* assign_stmt : assign_list) {
        type_to_entity_map_[DesignEntity::kAssign].push_back(assign_stmt);
        StatementNumber stmt_number = *assign_stmt->GetStatementNumber();
        std::string name_string = std::to_string(stmt_number.GetNum());
        entity_string_to_type_map_[name_string] = DesignEntity::kAssign;
        entity_object_to_type_map_[assign_stmt] = DesignEntity::kAssign;
        assign_map_[name_string].push_back(assign_stmt);
        Variable* var = assign_stmt->GetVariable();
        auto* var_name = const_cast<VariableName*>(var->GetName());
        std::string variable_string = var_name->getName();
        assign_map_[variable_string].push_back(assign_stmt);
    }
}

void PKB::PopulateCallEntities(const std::list<CallEntity*>& call_list) {
    for (CallEntity* call_stmt : call_list) {
        type_to_entity_map_[DesignEntity::kCall].push_back(call_stmt);
        StatementNumber stmt_number = *call_stmt->GetStatementNumber();
        std::string name_string = std::to_string(stmt_number.GetNum());
        entity_string_to_type_map_[name_string] = DesignEntity::kCall;
        entity_object_to_type_map_[call_stmt] = DesignEntity::kCall;
    }
}

void PKB::PopulatePrintEntities(const std::list<PrintEntity*>& print_list) {
    for (PrintEntity* print_stmt : print_list) {
        type_to_entity_map_[DesignEntity::kPrint].push_back(print_stmt);
        StatementNumber stmt_number = *print_stmt->GetStatementNumber();
        std::string name_string = std::to_string(stmt_number.GetNum());
        entity_string_to_type_map_[name_string] = DesignEntity::kPrint;
        entity_object_to_type_map_[print_stmt] = DesignEntity::kPrint;
    }
}

void PKB::PopulateReadEntities(const std::list<ReadEntity*>& read_list) {
    for (ReadEntity* read_stmt : read_list) {
        type_to_entity_map_[DesignEntity::kRead].push_back(read_stmt);
        StatementNumber stmt_number = *read_stmt->GetStatementNumber();
        std::string name_string = std::to_string(stmt_number.GetNum());
        entity_string_to_type_map_[name_string] = DesignEntity::kRead;
        entity_object_to_type_map_[read_stmt] = DesignEntity::kRead;
    }
}

DesignEntity PKB::GetTypeFromEntityString(std::string entity_string) {
    if (entity_string_to_type_map_.find(entity_string) == entity_string_to_type_map_.end()) {
        return DesignEntity::kInvalid;
    } else {
        return entity_string_to_type_map_[entity_string];
    }
}

DesignEntity PKB::GetTypeFromEntity(Entity* entity) {
    if (entity_object_to_type_map_.find(entity) == entity_object_to_type_map_.end()) {
        return DesignEntity::kInvalid;
    } else {
        return entity_object_to_type_map_[entity];
    }
}

std::vector<Entity*> PKB::GetDesignEntities(DesignEntity de) {
    return type_to_entity_map_[de];
}

std::vector<Entity*> PKB::GetAssignEntityByStmtRef(std::string stmtRef) {
    return assign_map_[stmtRef];
}

std::vector<Entity*> PKB::GetAssignEntityByVariable(std::string variable) {
    return assign_map_[variable];
}

std::vector<Entity*> PKB::GetWhileEntityByStmtRef(std::string stmtRef) {
    return while_map_[stmtRef];
}

std::vector<Entity*> PKB::GetWhileEntityByVariable(std::string variable) {
    return while_map_[variable];
}

std::vector<Entity*> PKB::GetIfEntityByStmtRef(std::string stmtRef) {
    return if_map_[stmtRef];
}

std::vector<Entity*> PKB::GetIfEntityByVariable(std::string variable) {
    return if_map_[variable];
}

bool PKB::HasRelationship(PKBRelRefs ref) {
    return !relationship_table_[ref].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
    return !relationship_by_type_table_[ref][{d1, d2}].empty();
}

bool PKB::HasRelationship(PKBRelRefs ref, std::string e1, std::string e2) {
    return relationship_set_.find({ref, e1, e2}) != relationship_set_.end();
}

void PKB::PopulateFollows(std::unordered_map<Statement*, Statement*>& follow_hash) {
    for (std::pair<Statement*, Statement*> kv: follow_hash) {
        auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string k_string = std::to_string(k_number->GetNum());
        relationship_table_[PKBRelRefs::kFollows][k_string].push_back(kv.second);
    }
}

void PKB::PopulateFollowedBy(std::unordered_map<Statement*, Statement*>& followed_by_hash) {
    for (std::pair<Statement*, Statement*> kv: followed_by_hash) {
        auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string k_string = std::to_string(k_number->GetNum());
        relationship_table_[PKBRelRefs::kFollowedBy][k_string].push_back(kv.second);
    }
}

void PKB::PopulateChild(std::unordered_map<Statement*, Statement*>& child_to_parent_hash) {
    for (std::pair<Statement*, Statement*> kv: child_to_parent_hash) {
        auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string k_string = std::to_string(k_number->GetNum());
        relationship_table_[PKBRelRefs::kChild][k_string].push_back(kv.second);
    }
}

void PKB::PopulateRelationship(std::unordered_map<Entity*, std::list<Entity*>*>* hash, PKBRelRefs ref) {
    for (std::pair<Entity*, std::list<Entity*>*> kv: *hash) {
        std::string k_string;

        if (dynamic_cast<Statement*>(kv.first) != nullptr) {
            Statement* stmt = dynamic_cast<Statement*>(kv.first);
            auto* k_number = const_cast<StatementNumber*>(stmt->GetStatementNumber());
            k_string = std::to_string(k_number->GetNum());
        } else if (dynamic_cast<Variable*>(kv.first) != nullptr) {
            Variable* var = dynamic_cast<Variable*>(kv.first);
            VariableName* variable_name = const_cast<VariableName*>(var->GetName());
            k_string = variable_name->getName();
        } else if (dynamic_cast<Procedure*>(kv.first) != nullptr) {
            Procedure* proc = dynamic_cast<Procedure*>(kv.first);
            ProcedureName* proc_name = const_cast<ProcedureName*>(proc->GetName());
            k_string = proc_name->getName();
        }
        for (Entity* entity : *kv.second) {
            relationship_set_.insert({ref, k_string, GetNameFromEntity(entity)});
            relationship_table_[ref][k_string].push_back(entity);
            DesignEntity first_type = EntityToDesignEntity(kv.first);
            DesignEntity second_type = EntityToDesignEntity(entity);
            relationship_by_type_table_[ref][{first_type, second_type}].push_back({kv.first, entity});
            if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[ref][{DesignEntity::kStmt, second_type}].push_back({kv.first, entity});
            }
            if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[ref][{first_type, DesignEntity::kStmt}].push_back({kv.first, entity});
                if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                    relationship_by_type_table_[ref][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back({kv.first, entity});
                }
            }
        }
    }
}

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
            DesignEntity first_type = EntityToDesignEntity(first_entity);
            DesignEntity second_type = EntityToDesignEntity(entity);
            relationship_by_type_table_[PKBRelRefs::kUsesC][{first_type, second_type}].push_back({first_entity, entity});
            if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kUsesC][{DesignEntity::kStmt, second_type}].push_back({first_entity, entity});
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
            DesignEntity first_type = EntityToDesignEntity(first_entity);
            DesignEntity second_type = EntityToDesignEntity(entity);
            relationship_by_type_table_[PKBRelRefs::kUsedByC][{first_type, second_type}].push_back({first_entity, entity});
            if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kUsedByC][{first_type, DesignEntity::kStmt}].push_back({first_entity, entity});
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
            DesignEntity first_type = EntityToDesignEntity(first_entity);
            DesignEntity second_type = EntityToDesignEntity(entity);
            relationship_by_type_table_[PKBRelRefs::kModifiesContainer][{first_type, second_type}].push_back({first_entity, entity});
            if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kModifiesContainer][{DesignEntity::kStmt, second_type}].push_back({first_entity, entity});
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
            DesignEntity first_type = EntityToDesignEntity(first_entity);
            DesignEntity second_type = EntityToDesignEntity(entity);
            relationship_by_type_table_[PKBRelRefs::kModifiedByContainer][{first_type, second_type}].push_back({first_entity, entity});
            if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kModifiedByContainer][{first_type, DesignEntity::kStmt}].push_back({first_entity, entity});
            }
        }
    }
}

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
        return "c" + std::to_string(cv->Get());
    }
}


DesignEntity PKB::EntityToDesignEntity(Entity* entity) {
    EntityEnum entity_enum = entity->getEntityEnum();
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
    }
}


// TO BE DEPRECATED FROM HERE

/**
 * Populates the PKB proc_table_ and adds type info to type_map_
 * @param proc_list is a list of Procedure pointers
 * @return
 */
void PKB::PopulateProcList(const std::list<Procedure*>& proc_list) {
  proc_table_ = std::list<std::string>();
  for (auto const& i: proc_list) {
    auto* proc_name = const_cast<ProcedureName*>(i->GetName());
    proc_table_.push_back(proc_name->getName());
    type_map_[proc_name->getName()] = DesignEntity::kProcedure;
  }
}

/**
 * Populates the PKB var_table_ and adds type info to type_map_
 * @param var_list is a list of Variable pointers
 * @return
 */
void PKB::PopulateVarList(const std::list<Variable*>& var_list) {
  var_table_ = std::list<std::string>();
  for (auto const& i: var_list) {
    auto* var_name = const_cast<VariableName*>(i->GetName());
    var_table_.push_back(var_name->getName());
    type_map_[var_name->getName()] = DesignEntity::kVariable;
  }
}

/**
 * Populates the PKB const_table_ and adds type info to type_map_
 * @param const_list is a list of ConstantValue pointers
 * @return
 */
void PKB::PopulateConstList(const std::list<ConstantValue*>& const_list) {
  const_table_ = std::list<std::string>();
  for (auto const& i: const_list) {
    int const_value = i->Get();
    const_table_.push_back(std::to_string(const_value));
    type_map_[std::to_string(const_value)] = DesignEntity::kConstant;
  }
}

/**
 * Populates the PKB stmt_table_
 * @param stmt_list is a list of Statement pointers
 * @return
 */
void PKB::PopulateStmtList(const std::list<Statement*>& stmt_list) {
  stmt_table_ = std::list<std::string>();
  for (auto const& i: stmt_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    stmt_table_.push_back(std::to_string(s_number->GetNum()));
  }
}

/**
 * Populates the PKB if_table_ and adds type info to type_map_
 * @param if_list is a list of IfEntity pointers
 * @return
 */
void PKB::PopulateIfList(const std::list<IfEntity*>& if_list) {
  if_table_ = std::list<std::string>();
  for (auto const& i: if_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    if_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kIf;
  }
}

/**
 * Populates the PKB while_table_ and adds type info to type_map_
 * @param while_list is a list of WhileEntity pointers
 * @return
 */
void PKB::PopulateWhileList(const std::list<WhileEntity*>& while_list) {
  while_table_ = std::list<std::string>();
  for (auto const& i: while_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    while_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kWhile;
  }
}

/**
 * Populates the PKB assign_table_, adds type info to type_map_, and populates assign_expr_map_
 * @param assign_list is a list of AssignEntity pointers
 * @return
 */
void PKB::PopulateAssignList(const std::list<AssignEntity*>& assign_list) {
  assign_table_ = std::list<std::string>();
  for (auto const& i: assign_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    assign_table_.push_back(std::to_string(s_number->GetNum()));

    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kAssign;

    // Populate assign_expr_map_ with AssignEntity

    AssignEntity entity = * const_cast<AssignEntity*>(i);
    std::vector<AssignEntity> stmt_map_vector{entity};
    std::vector<AssignEntity> var_map_vector{entity};
    assign_expr_map_[std::to_string(s_number->GetNum())] = stmt_map_vector;

    VariableName var_name = * entity.GetVariable()->GetName();
    if (assign_expr_map_.find(var_name.getName()) != assign_expr_map_.end()) {
      assign_expr_map_[var_name.getName()].push_back(entity);
    } else {
      assign_expr_map_[var_name.getName()] = var_map_vector;
    }
  }
}

/**
 * Populates the PKB call_table_ and adds type info to type_map_
 * @param call_list is a list of CallEntity pointers
 * @return
 */
void PKB::PopulateCallList(const std::list<CallEntity*>& call_list) {
  call_table_ = std::list<std::string>();
  for (auto const& i: call_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    call_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kCall;
  }
}

/**
 * Populates the PKB print_table_ and adds type info to type_map_
 * @param print_list is a list of PrintEntity pointers
 * @return
 */
void PKB::PopulatePrintList(const std::list<PrintEntity*>& print_list) {
  print_table_ = std::list<std::string>();
  for (auto const& i: print_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    print_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kPrint;
  }
}

/**
 * Populates the PKB read_table_ and adds type info to type_map_
 * @param read_list is a list of ReadEntity pointers
 * @return
 */
void PKB::PopulateReadList(const std::list<ReadEntity*>& read_list) {
  read_table_ = std::list<std::string>();
  for (auto const& i: read_list) {
    auto* s_number = const_cast<StatementNumber*>(i->GetStatementNumber());
    read_table_.push_back(std::to_string(s_number->GetNum()));
    type_map_[std::to_string(s_number->GetNum())] = DesignEntity::kRead;
  }
}

/**
 * Populates the PKB follows_map_
 * @param follow_hash is a hashmap representing follows relationships between statements
 * @return
 */
void PKB::PopulateFollowsMap(const std::unordered_map<Statement*, Statement*>& follow_hash) {
  for (std::pair<Statement*, Statement*> kv: follow_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto* v_number = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
    std::string v_string = std::to_string(v_number->GetNum());
    DesignEntity v_type = type_map_.find(v_string)->second;
    std::tuple<DesignEntity, std::string> result = make_tuple(v_type, v_string);
    follows_map_[k_string] = result;
  }
}

/**
 * Populates the PKB previous_map_
 * @param followed_by_hash is a hashmap representing followed by relationships between statements
 * @return
 */
void PKB::PopulatePreviousMap(const std::unordered_map<Statement*, Statement*>& followed_by_hash) {
  for (std::pair<Statement*, Statement*> kv: followed_by_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto* v_number = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
    std::string v_string = std::to_string(v_number->GetNum());
    DesignEntity v_type = type_map_.find(v_string)->second;
    std::tuple<DesignEntity, std::string> result = make_tuple(v_type, v_string);
    previous_map_[k_string] = result;
  }
}

/**
 * Populates the PKB parent_map_
 * @param parent_to_child_hash is a hashmap representing parent relationships between statements
 * @return
 */
void PKB::PopulateParentMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: parent_to_child_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* children = kv.second;

    std::string stmt_ref;

    for (Statement* child: * children) {
      auto* childNumber = const_cast<StatementNumber*>(child->GetStatementNumber());
      stmt_ref = std::to_string(childNumber->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    parent_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB child_map_
 * @param child_to_parent_hash_ is a hashmap representing child relationships between statements
 * @return
 */
void PKB::PopulateChildMap(const std::unordered_map<Statement*, Statement*>& child_to_parent_hash_) {
  for (std::pair<Statement*, Statement*> kv: child_to_parent_hash_) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto* v_number = const_cast<StatementNumber*>(kv.second->GetStatementNumber());
    std::string v_string = std::to_string(v_number->GetNum());
    DesignEntity v_type = type_map_.find(v_string)->second;
    std::tuple<DesignEntity, std::string> result = make_tuple(v_type, v_string);
    child_map_[k_string] = result;
  }
}

/**
 * Populates the PKB follows_t_map_
 * @param follow_T_hash is a hashmap representing follows* relationships between statements
 * @return
 */
void PKB::PopulateFollowsTMap(std::unordered_map<Statement*, std::list<Statement*>*> follows_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: follows_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* followsList = kv.second;

    std::string stmt_ref;

    for (Statement* follows: * followsList) {
      auto* follows_number = const_cast<StatementNumber*>(follows->GetStatementNumber());
      stmt_ref = std::to_string(follows_number->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    follows_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB previous_t_map_
 * @param previous_T_hash is a hashmap representing previous* relationships between statements
 * @return
 */
void PKB::PopulatePreviousTMap(std::unordered_map<Statement*, std::list<Statement*>*> previous_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: previous_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* previousList = kv.second;

    std::string stmt_ref;

    for (Statement* previous: * previousList) {
      auto* prev_number = const_cast<StatementNumber*>(previous->GetStatementNumber());
      stmt_ref = std::to_string(prev_number->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    previous_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB parent_t_map_
 * @param parent_to_child_T_hash is a hashmap representing parent* relationships between statements
 * @return
 */
void PKB::PopulateParentTMap(std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: parent_to_child_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* children = kv.second;

    std::string stmt_ref;

    for (Statement* child: * children) {
      auto* childNumber = const_cast<StatementNumber*>(child->GetStatementNumber());
      stmt_ref = std::to_string(childNumber->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    parent_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB child_t_map_
 * @param child_to_parent_T_hash is a hashmap representing child* relationships between statements
 * @return
 */
void PKB::PopulateChildTMap(std::unordered_map<Statement*, std::list<Statement*>*> child_to_parent_T_hash) {
  for (std::pair<Statement*, std::list<Statement*>*> kv: child_to_parent_T_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* parents = kv.second;

    std::string stmt_ref;

    for (Statement* parent: * parents) {
      auto* parentNumber = const_cast<StatementNumber*>(parent->GetStatementNumber());
      stmt_ref = std::to_string(parentNumber->GetNum());
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }

    child_t_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB use_s_map_
 * @param use_hash is a hashmap representing use relationships between non-container statements and variables
 * @return
 */
void PKB::PopulateUseSMap(std::unordered_map<Statement*, std::list<Variable*>*> use_hash) {
  for (std::pair<Statement*, std::list<Variable*>*> kv: use_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    use_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB used_by_s_map_
 * @param used_by_hash is a hashmap representing used by relationships between variables and non-container statements
 * @return
 */
void PKB::PopulateUsedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> used_by_hash) {
  for (std::pair<Variable*, std::list<Statement*>*> kv: used_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* using_statements = kv.second;

    for (Statement* stmt: * using_statements) {
      auto* stmt_num = const_cast<StatementNumber*>(stmt->GetStatementNumber());
      std::string stmt_ref = std::to_string(stmt_num->GetNum());

      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }
    used_by_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB use_c_map_
 * @param c_use_hash is a hashmap representing use relationships between container statements and variables
 * @return
 */
void PKB::PopulateUseCMap(std::unordered_map<Container*, std::list<Variable*>*> c_use_hash) {
  for (std::pair<Container*, std::list<Variable*>*> kv: c_use_hash) {

      std::string k_string;
      if (dynamic_cast<Statement*>(kv.first) != nullptr) {
          Statement* c_stmt = dynamic_cast<Statement*>(kv.first);
          auto* k_number = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
          k_string = std::to_string(k_number->GetNum());
      } else if (dynamic_cast<Procedure*>(kv.first) != nullptr) {
          Procedure* proc = dynamic_cast<Procedure*>(kv.first);
          auto* k_number = const_cast<ProcedureName*>(proc->GetName());
          k_string = k_number->getName();
      }

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    use_c_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB used_by_c_map_
 * @param c_used_by_hash is a hashmap representing used by relationships between variables and container statements
 * @return
 */
void PKB::PopulateUsedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_used_by_hash) {
  for (std::pair<Variable*, std::list<Container*>*> kv: c_used_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Container*>* using_statements = kv.second;

    for (Container* stmt: * using_statements) {
      std::string ref;
      if (Statement* c_stmt = dynamic_cast<Statement*>(stmt)) {
          auto* stmt_num = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
          ref = std::to_string(stmt_num->GetNum());
      } else if (Procedure* proc = dynamic_cast<Procedure*>(stmt)) {
          auto* k_number = const_cast<ProcedureName*>(proc->GetName());
          ref = k_number->getName();
      }

      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[ref], ref);
      result_ptr->push_back(entity_ptr);
    }

    used_by_c_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modifies_s_map_
 * @param modifies_hash is a hashmap representing modifies relationships between non-container statements and variables
 * @return
 */
void PKB::PopulateModifiesSMap(std::unordered_map<Statement*, std::list<Variable*>*> modifies_hash) {
  for (std::pair<Statement*, std::list<Variable*>*> kv: modifies_hash) {
    auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
    std::string k_string = std::to_string(k_number->GetNum());
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    modifies_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modified_by_s_map_
 * @param modified_by_hash is a hashmap representing modified by relationships between variables and non-container statements
 * @return
 */
void PKB::PopulateModifiedBySMap(std::unordered_map<Variable*, std::list<Statement*>*> modified_by_hash) {
  for (std::pair<Variable*, std::list<Statement*>*> kv: modified_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Statement*>* using_statements = kv.second;

    for (Statement* stmt: * using_statements) {
      auto* stmt_num = const_cast<StatementNumber*>(stmt->GetStatementNumber());
      std::string stmt_ref = std::to_string(stmt_num->GetNum());

      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[stmt_ref], stmt_ref);
      result_ptr->push_back(entity_ptr);
    }
    modified_by_s_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modifies_c_map_
 * @param c_modifies_hash is a hashmap representing modifies relationships between container statements and variables
 * @return
 */
void PKB::PopulateModifiesCMap(std::unordered_map<Container*, std::list<Variable*>*> c_modifies_hash) {
  for (std::pair<Container*, std::list<Variable*>*> kv: c_modifies_hash) {
      std::string k_string;
      if (dynamic_cast<Statement*>(kv.first) != nullptr) {
          Statement* c_stmt = dynamic_cast<Statement*>(kv.first);
          auto* k_number = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
          k_string = std::to_string(k_number->GetNum());
      } else if (dynamic_cast<Procedure*>(kv.first) != nullptr) {
          Procedure* proc = dynamic_cast<Procedure*>(kv.first);
          auto* k_number = const_cast<ProcedureName*>(proc->GetName());
          k_string = k_number->getName();
      }
    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Variable*>* used_variables = kv.second;

    std::string var_string;

    for (Variable* var: * used_variables) {
      VariableName* var_name = const_cast<VariableName*>(var->GetName());
      var_string = var_name->getName();
      std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(DesignEntity::kVariable, var_string);
      result_ptr->push_back(entity_ptr);
    }

    modifies_c_map_[k_string] = result_ptr;
  }
}

/**
 * Populates the PKB modified_by_c_map_
 * @param c_modified_by_hash is a hashmap representing modified by relationships between variables and container statements
 * @return
 */
void PKB::PopulateModifiedByCMap(std::unordered_map<Variable*, std::list<Container*>*> c_modified_by_hash) {
  for (std::pair<Variable*, std::list<Container*>*> kv: c_modified_by_hash) {

    VariableName* var_name = const_cast<VariableName*>(kv.first->GetName());
    std::string k_string = var_name->getName();

    auto result_ptr = new std::list<std::tuple<DesignEntity, std::string>*>();

    std::list<Container*>* using_statements = kv.second;

    for (Container* stmt: * using_statements) {
        std::string ref;
        if (Statement* c_stmt = dynamic_cast<Statement*>(stmt)) {
            auto* stmt_num = const_cast<StatementNumber*>(c_stmt->GetStatementNumber());
            ref = std::to_string(stmt_num->GetNum());
        } else if (Procedure* proc = dynamic_cast<Procedure*>(stmt)) {
            auto* k_number = const_cast<ProcedureName*>(proc->GetName());
            ref = k_number->getName();
        }

        std::tuple<DesignEntity, std::string>* entity_ptr = new std::tuple(type_map_[ref], ref);
      result_ptr->push_back(entity_ptr);
    }

    modified_by_c_map_[k_string] = result_ptr;
  }
}

/**
 * Returns a list of strings representing elements of the specified DesignEntity type
 * @param de is an enum of the types of DesignEntity (e.g. read, print, assign, etc.)
 * @return list of strings in which each string is the identifier of an element
 */
std::list<std::string> PKB::GetDesignEntity(DesignEntity de) {
    std::list<std::string> result = std::list<std::string>();
    switch (de) {
        case DesignEntity::kStmt:return stmt_table_;
        case DesignEntity::kRead:return read_table_;
        case DesignEntity::kPrint:return print_table_;
        case DesignEntity::kCall:return call_table_;
        case DesignEntity::kWhile:return while_table_;
        case DesignEntity::kIf:return if_table_;
        case DesignEntity::kAssign:return assign_table_;
        case DesignEntity::kVariable:return var_table_;
        case DesignEntity::kConstant:return const_table_;
        case DesignEntity::kProcedure:return proc_table_;
        case DesignEntity::kInvalid:return {};
        default:return {};
    }
}

/**
 * Returns boolean of whether there exists any follows relationship
 * @param
 * @return True if there is at least one follows relationship, otherwise False
 */
bool PKB::HasFollows() {
    return !follows_map_.empty();
};

/**
 * Returns boolean of whether there exists any Previous relationship
 * @param
 * @return True if there is at least one previous relationship, otherwise False
 */
bool PKB::HasPrevious() {
    return !previous_map_.empty();
}

/**
 * Returns boolean of whether there exists any parent relationship
 * @param
 * @return True if there is at least one parent relationship, otherwise False
 */
bool PKB::HasParent() {
    return !parent_map_.empty();
}

/**
 * Returns boolean of whether there exists any child relationship
 * @param
 * @return True if there is at least one child relationship, otherwise False
 */
bool PKB::HasChild() {
    return !child_map_.empty();
}

/**
 * Returns boolean of whether there exists any follows* relationship
 * @param
 * @return True if there is at least one follows* relationship, otherwise False
 */
bool PKB::HasFollowsT() {
    return !follows_t_map_.empty();
};

/**
 * Returns boolean of whether there exists any previous* relationship
 * @param
 * @return True if there is at least one previous* relationship, otherwise False
 */
bool PKB::HasPreviousT() {
    return !previous_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any parent* relationship
 * @param
 * @return True if there is at least one parent* relationship, otherwise False
 */
bool PKB::HasParentT() {
    return !parent_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any child* relationship
 * @param
 * @return True if there is at least one child* relationship, otherwise False
 */
bool PKB::HasChildT() {
    return !child_t_map_.empty();
}

/**
 * Returns boolean of whether there exists any uses relationship
 * @param
 * @return True if there is at least one uses relationship, otherwise False
 */
bool PKB::HasUses() {
    // Must check if both container and statement maps are empty
    return !use_s_map_.empty() || !use_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any used by relationship
 * @param
 * @return True if there is at least one used by relationship, otherwise False
 */
bool PKB::HasUsedBy() {
    // Must check if both container and statement maps are empty
    return !used_by_s_map_.empty() || !used_by_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any modifies relationship
 * @param
 * @return True if there is at least one modifies relationship, otherwise False
 */
bool PKB::HasModifies() {
    // Must check if both container and statement maps are empty
    return !modifies_s_map_.empty() || !modifies_c_map_.empty();
}

/**
 * Returns boolean of whether there exists any modified by relationship
 * @param
 * @return True if there is at least one modified by relationship, otherwise False
 */
bool PKB::HasModifiedBy() {
    // Must check if both container and statement maps are empty
    return !modified_by_s_map_.empty() || !modified_by_c_map_.empty();
}


/**
 * Returns a list of tuples representing elements that fulfill the follows relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetFollows(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto follows_iter = follows_map_.find(stmt);
    if (follows_iter == follows_map_.end()) {
        return ret_list;
    } else {
        std::tuple<DesignEntity, std::string> follows = follows_iter->second;
        ret_list.push_back(follows);
        return ret_list;
    }
}

/**
 * Returns a list of tuples representing elements that fulfill the previous relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetPrevious(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto previous_iter = previous_map_.find(stmt);
    if (previous_iter == previous_map_.end()) {
        return ret_list;
    } else {
        std::tuple<DesignEntity, std::string> previous = previous_iter->second;
        ret_list.push_back(previous);
        return ret_list;
    }
}

/**
 * Returns a list of tuples representing elements that fulfill the parent relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetParent(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto parent_iter = parent_map_.find(stmt);
    if (parent_iter != parent_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* children = parent_iter->second;
        for (auto s: * children) {
            ret_list.push_back(* s);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the child relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetChild(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto child_iter = child_map_.find(stmt);
    if (child_iter == child_map_.end()) {
        return ret_list;
    } else {
        std::tuple<DesignEntity, std::string> child = child_iter->second;
        ret_list.push_back(child);
        return ret_list;
    }
}

/**
 * Returns a list of tuples representing elements that fulfill the follows* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetFollowsT(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto follows_iter = follows_t_map_.find(stmt);
    if (follows_iter != follows_t_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* before = follows_iter->second;
        for (auto s: * before) {
            ret_list.push_back(* s);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the previous* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetPreviousT(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto previous_iter = previous_t_map_.find(stmt);
    if (previous_iter != previous_t_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* after = previous_iter->second;
        for (auto s: * after) {
            ret_list.push_back(* s);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the parent* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetParentT(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto parent_iter = parent_t_map_.find(stmt);
    if (parent_iter != parent_t_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* children = parent_iter->second;
        for (auto s: * children) {
            ret_list.push_back(* s);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the child* relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetChildT(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    auto child_iter = child_t_map_.find(stmt);
    if (child_iter != child_t_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* parents = child_iter->second;
        for (auto s: * parents) {
            ret_list.push_back(* s);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the uses relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUses(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For statement map
    auto use_s_iter = use_s_map_.find(stmt);
    if (use_s_iter != use_s_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* use_s = use_s_iter->second;
        for (auto v: * use_s) {
            ret_list.push_back(* v);
        }
    }
    // For container map
    auto use_c_iter = use_c_map_.find(stmt);
    if (use_c_iter != use_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* use_c = use_c_iter->second;
        for (auto v: * use_c) {
            ret_list.push_back(* v);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the used by relationship with the specified statement
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUsedBy(std::string var) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For statement map
    auto used_by_s_iter = used_by_s_map_.find(var);
    if (used_by_s_iter != used_by_s_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* used_by_s = used_by_s_iter->second;
        for (auto s: * used_by_s) {
            ret_list.push_back(* s);
        }
    }
    // For container map
    auto used_by_c_iter = used_by_c_map_.find(var);
    if (used_by_c_iter != used_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* used_by_c = used_by_c_iter->second;
        for (auto c: * used_by_c) {
            ret_list.push_back(* c);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modifies relationship with the specified statement
 * @param stmt is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifies(std::string stmt) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For statement map
    auto modifies_s_iter = modifies_s_map_.find(stmt);
    if (modifies_s_iter != modifies_s_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modifies_s = modifies_s_iter->second;
        for (auto v: * modifies_s) {
            ret_list.push_back(* v);
        }
    }
    // For container map
    auto modifies_c_iter = modifies_c_map_.find(stmt);
    if (modifies_c_iter != modifies_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modifies_c = modifies_c_iter->second;
        for (auto v: * modifies_c) {
            ret_list.push_back(* v);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modified by relationship with the specified statement
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and statement number (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifiedBy(std::string var) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For statement map
    auto modified_by_s_iter = modified_by_s_map_.find(var);
    if (modified_by_s_iter != modified_by_s_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modified_by_s = modified_by_s_iter->second;
        for (auto s: * modified_by_s) {
            ret_list.push_back(* s);
        }
    }
    // For container map
    auto modified_by_c_iter = modified_by_c_map_.find(var);
    if (modified_by_c_iter != modified_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modified_by_c = modified_by_c_iter->second;
        for (auto c: * modified_by_c) {
            ret_list.push_back(* c);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the uses relationship with the specified procedure
 * @param procedure is a string representing the procedure
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUsesP(std::string procedure) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto use_c_iter = use_c_map_.find(procedure);
    if (use_c_iter != use_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* use_c = use_c_iter->second;
        for (auto v: * use_c) {
            ret_list.push_back(* v);
        }
    }
    return ret_list;
}

/**
 * Returns a list of procedures representing elements that fulfill the used by relationship with the specified variable
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and procedure name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetUsedByP(std::string var) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto used_by_c_iter = used_by_c_map_.find(var);
    if (used_by_c_iter != used_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* used_by_c = used_by_c_iter->second;
        for (auto c: * used_by_c) {
            if (std::get<0>(*c) == DesignEntity::kProcedure) {
                ret_list.push_back(*c);
            }
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modifies relationship with the specified procedure
 * @param procedure is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and variable name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifiesP(std::string procedure) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto modifies_c_iter = modifies_c_map_.find(procedure);
    if (modifies_c_iter != modifies_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modifies_c = modifies_c_iter->second;
        for (auto v: * modifies_c) {
            ret_list.push_back(* v);
        }
    }
    return ret_list;
}

/**
 * Returns a list of tuples representing elements that fulfill the modified by relationship with the specified variable
 * @param var is a string representing the variable name (e.g. "i", "byte", "u", ...)
 * @return list of tuples; each tuple consists of its type (DesignEntity) and procedure name (string)
 */
std::list<std::tuple<DesignEntity, std::string>> PKB::GetModifiedByP(std::string var) {
    std::list<std::tuple<DesignEntity, std::string>> ret_list = std::list<std::tuple<DesignEntity, std::string>>();
    // For container map
    auto modified_by_c_iter = modified_by_c_map_.find(var);
    if (modified_by_c_iter != modified_by_c_map_.end()) {
        std::list<std::tuple<DesignEntity, std::string>*>* modified_by_c = modified_by_c_iter->second;
        for (auto c: * modified_by_c) {
            if (std::get<0>(*c) == DesignEntity::kProcedure) {
                ret_list.push_back(*c);
            }
        }
    }
    return ret_list;
}

/**
 * From an assign statement, Get its AssignEntity object
 * @param stmt_ref is a string representing the statement number (e.g. "1", "2", "3", ...)
 * @return vector containing one AssignEntity object
 */
std::vector<AssignEntity> PKB::GetPatternByAssign(std::string stmt_ref) {
    return assign_expr_map_[stmt_ref];
}

/**
 * From a variable name, Get all AssignEntity object with the variable on the LHS
 * @param var_name is a string representing the variable name (e.g. "give", "me", "A", ...)
 * @return vector containing the AssignEntity objects associated with var_name
 */
std::vector<AssignEntity> PKB::GetPatternByVariable(std::string var_name) {
    return assign_expr_map_[var_name];
}

// TO HERE