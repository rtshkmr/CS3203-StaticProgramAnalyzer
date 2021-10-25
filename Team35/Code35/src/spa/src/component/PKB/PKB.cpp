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
      PKBRelRefs::kUsedByS,
      PKBRelRefs::kModifiesStatement,
      PKBRelRefs::kModifiedByStatement,
      PKBRelRefs::kCalls,
      PKBRelRefs::kCalledBy,
      PKBRelRefs::kCallsT,
      PKBRelRefs::kCalledByT,
      PKBRelRefs::kNext,
      PKBRelRefs::kPrevious,
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
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.calls_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.called_by_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.calls_T_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.called_by_T_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.next_hash_));
  non_proc_hashes.push_back(reinterpret_cast<std::unordered_map<Entity*, std::list<Entity*>*>*>(&d.previous_hash_));


  for (int i = 0; i < non_proc_hashes.size(); i++) {
      PopulateRelationship(non_proc_hashes.at(i), non_proc_refs_to_populate.at(i));
  }

  PopulateContainerUse(d.container_use_hash_);
  PopulateContainerUsedBy(d.container_used_by_hash_);
  PopulateContainerModifies(d.container_modifies_hash_);
  PopulateContainerModifiedBy(d.container_modified_by_hash_);

  PopulateUses();
  PopulateModifies();
  PopulateUsedBy();
  PopulateModifiedBy();

  L("[DONE] PKB POPULATED WITHOUT ERROR\n");
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

    return relationship_by_type_table_[ref][{d1, d2}];
}

std::vector<Entity*> PKB::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity d1, DesignEntity d2) {
  if (d1 == DesignEntity::kProgLine) {
    d1 = DesignEntity::kStmt;
  }
  if (d2 == DesignEntity::kProgLine) {
    d2 = DesignEntity::kStmt;
  }

  return entities_in_relationship_by_types_table_[ref][{d1, d2}];
}

std::vector<Entity*> PKB::GetRelationshipByType(PKBRelRefs ref, DesignEntity d) {
    if (d == DesignEntity::kProgLine) {
      d = DesignEntity::kStmt;
    }
    if (second_param_is_stmt.find(ref) != second_param_is_stmt.end()) {
      return entities_in_relationship_by_types_table_[ref][{d, DesignEntity::kStmt}];
    } else {
      if (second_param_is_var.find(ref) != second_param_is_var.end()) {
        return entities_in_relationship_by_types_table_[ref][{d, DesignEntity::kVariable}];
      } else if (second_param_is_proc.find(ref) != second_param_is_proc.end()) {
        return entities_in_relationship_by_types_table_[ref][{d, DesignEntity::kProcedure}];
      }
    }
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

void PKB::PopulateStmtEntities(const std::vector<Statement*> &stmt_list) {
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
        for (Variable* var : ifs->GetControlVariables()) {
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
        for (Variable* var : while_stmt->GetControlVariables()) {
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

std::vector<Entity*> PKB::GetDesignEntities(DesignEntity de) {
    if (de == DesignEntity::kProgLine) {
      de = DesignEntity::kStmt;
    }
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
        DesignEntity first_type = EntityToDesignEntity(kv.first);
        DesignEntity second_type = EntityToDesignEntity(kv.second);

        relationship_by_type_table_[PKBRelRefs::kFollows][{first_type, second_type}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kFollows][{DesignEntity::kStmt, second_type}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kFollows][{first_type, DesignEntity::kStmt}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kFollows][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back(
            {kv.first, kv.second}
        );

        entities_in_relationship_by_types_table_[PKBRelRefs::kFollows][{first_type, second_type}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kFollows][{DesignEntity::kStmt, second_type}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kFollows][{first_type, DesignEntity::kStmt}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kFollows][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back(
          kv.first
          );

        relationship_set_.insert({
            PKBRelRefs::kFollows,
            std::to_string(kv.first->GetStatementNumber()->GetNum()),
            std::to_string(kv.second->GetStatementNumber()->GetNum())
        });
    }
}

void PKB::PopulateFollowedBy(std::unordered_map<Statement*, Statement*>& followed_by_hash) {
    for (std::pair<Statement*, Statement*> kv: followed_by_hash) {
        auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string k_string = std::to_string(k_number->GetNum());
        relationship_table_[PKBRelRefs::kFollowedBy][k_string].push_back(kv.second);
        DesignEntity first_type = EntityToDesignEntity(kv.first);
        DesignEntity second_type = EntityToDesignEntity(kv.second);
        relationship_by_type_table_[PKBRelRefs::kFollowedBy][{first_type, second_type}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kFollowedBy][{DesignEntity::kStmt, second_type}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kFollowedBy][{first_type, DesignEntity::kStmt}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kFollowedBy][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back(
            {kv.first, kv.second}
        );

        entities_in_relationship_by_types_table_[PKBRelRefs::kFollowedBy][{first_type, second_type}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kFollowedBy][{DesignEntity::kStmt, second_type}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kFollowedBy][{first_type, DesignEntity::kStmt}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kFollowedBy][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back(
          kv.first
          );

        relationship_set_.insert({
            PKBRelRefs::kFollowedBy,
            std::to_string(kv.first->GetStatementNumber()->GetNum()),
            std::to_string(kv.second->GetStatementNumber()->GetNum())
        });
    }
}

void PKB::PopulateChild(std::unordered_map<Statement*, Statement*>& child_to_parent_hash) {
    for (std::pair<Statement*, Statement*> kv: child_to_parent_hash) {
        auto* k_number = const_cast<StatementNumber*>(kv.first->GetStatementNumber());
        std::string k_string = std::to_string(k_number->GetNum());
        relationship_table_[PKBRelRefs::kChild][k_string].push_back(kv.second);
        DesignEntity first_type = EntityToDesignEntity(kv.first);
        DesignEntity second_type = EntityToDesignEntity(kv.second);
        relationship_by_type_table_[PKBRelRefs::kChild][{first_type, second_type}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kChild][{DesignEntity::kStmt, second_type}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kChild][{first_type, DesignEntity::kStmt}].push_back(
            {kv.first, kv.second}
        );
        relationship_by_type_table_[PKBRelRefs::kChild][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back(
            {kv.first, kv.second}
        );

        entities_in_relationship_by_types_table_[PKBRelRefs::kChild][{first_type, second_type}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kChild][{DesignEntity::kStmt, second_type}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kChild][{first_type, DesignEntity::kStmt}].push_back(
          kv.first
          );
        entities_in_relationship_by_types_table_[PKBRelRefs::kChild][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back(
          kv.first
          );

        relationship_set_.insert({
            PKBRelRefs::kChild,
            std::to_string(kv.first->GetStatementNumber()->GetNum()),
            std::to_string(kv.second->GetStatementNumber()->GetNum())
        });
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
            entities_in_relationship_by_types_table_[ref][{first_type, second_type}].push_back(kv.first);

            if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[ref][{DesignEntity::kStmt, second_type}].push_back({kv.first, entity});
                entities_in_relationship_by_types_table_[ref][{DesignEntity::kStmt, second_type}].push_back(kv.first);
            }
            if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[ref][{first_type, DesignEntity::kStmt}].push_back({kv.first, entity});
                entities_in_relationship_by_types_table_[ref][{first_type, DesignEntity::kStmt}].push_back(kv.first);
                if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                    relationship_by_type_table_[ref][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back({kv.first, entity});
                    entities_in_relationship_by_types_table_[ref][{DesignEntity::kStmt, DesignEntity::kStmt}].push_back(kv.first);
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
            entities_in_relationship_by_types_table_[PKBRelRefs::kUsesC][{first_type, second_type}].push_back(first_entity);
            if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kUsesC][{DesignEntity::kStmt, second_type}].push_back({first_entity, entity});
                entities_in_relationship_by_types_table_[PKBRelRefs::kUsesC][{DesignEntity::kStmt, second_type}].push_back(first_entity);
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
            entities_in_relationship_by_types_table_[PKBRelRefs::kUsedByC][{first_type, second_type}].push_back(first_entity);
            if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kUsedByC][{first_type, DesignEntity::kStmt}].push_back({first_entity, entity});
                entities_in_relationship_by_types_table_[PKBRelRefs::kUsedByC][{first_type, DesignEntity::kStmt}].push_back(first_entity);
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
            entities_in_relationship_by_types_table_[PKBRelRefs::kModifiesContainer][{first_type, second_type}].push_back(first_entity);
            if (stmt_design_entities_.find(first_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kModifiesContainer][{DesignEntity::kStmt, second_type}].push_back({first_entity, entity});
                entities_in_relationship_by_types_table_[PKBRelRefs::kModifiesContainer][{DesignEntity::kStmt, second_type}].push_back(first_entity);
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
            entities_in_relationship_by_types_table_[PKBRelRefs::kModifiedByContainer][{first_type, second_type}].push_back(first_entity);
            if (stmt_design_entities_.find(second_type) != stmt_design_entities_.end()) {
                relationship_by_type_table_[PKBRelRefs::kModifiedByContainer][{first_type, DesignEntity::kStmt}].push_back({first_entity, entity});
                entities_in_relationship_by_types_table_[PKBRelRefs::kModifiedByContainer][{first_type, DesignEntity::kStmt}].push_back(first_entity);
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
        s_combo_map = relationship_by_type_table_[PKBRelRefs::kUsesS];

    std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
        c_combo_map = relationship_by_type_table_[PKBRelRefs::kUsesC];

    for (auto pair : s_combo_map) {
        relationship_by_type_table_[PKBRelRefs::kUses][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kUses][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );

        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kUses][pair.first].push_back(std::get<0>(entity_pairs));
        }
    }

    for (auto pair : c_combo_map) {
        if (std::get<0>(pair.first) == DesignEntity::kProcedure) {
            continue;
        }
        relationship_by_type_table_[PKBRelRefs::kUses][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kUses][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );

        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kUses][pair.first].push_back(std::get<0>(entity_pairs));
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
    s_combo_map = relationship_by_type_table_[PKBRelRefs::kModifiesStatement];

    std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
    c_combo_map = relationship_by_type_table_[PKBRelRefs::kModifiesContainer];

    for (auto pair : s_combo_map) {
        relationship_by_type_table_[PKBRelRefs::kModifies][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kModifies][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );
        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kModifies][pair.first].push_back(std::get<0>(entity_pairs));
        }
    }

    for (auto pair : c_combo_map) {
        if (std::get<0>(pair.first) == DesignEntity::kProcedure) {
            continue;
        }
        relationship_by_type_table_[PKBRelRefs::kModifies][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kModifies][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );
        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kModifies][pair.first].push_back(std::get<0>(entity_pairs));
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
    s_combo_map = relationship_by_type_table_[PKBRelRefs::kUsedByS];

    std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
    c_combo_map = relationship_by_type_table_[PKBRelRefs::kUsedByC];

    for (auto pair : s_combo_map) {
        relationship_by_type_table_[PKBRelRefs::kUsedBy][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kUsedBy][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );
        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
        }
    }

    for (auto pair : c_combo_map) {
        if (std::get<1>(pair.first) == DesignEntity::kProcedure) {
            continue;
        }
        relationship_by_type_table_[PKBRelRefs::kUsedBy][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kUsedBy][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );
        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
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
    s_combo_map = relationship_by_type_table_[PKBRelRefs::kModifiedByStatement];

    std::unordered_map<type_combo, std::vector<std::tuple<Entity*, Entity*>>, type_combo_hash>
    c_combo_map = relationship_by_type_table_[PKBRelRefs::kModifiedByContainer];

    for (auto pair : s_combo_map) {
        relationship_by_type_table_[PKBRelRefs::kModifiedBy][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kModifiedBy][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );
        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
        }
    }

    for (auto pair : c_combo_map) {
        if (std::get<1>(pair.first) == DesignEntity::kProcedure) {
            continue;
        }
        relationship_by_type_table_[PKBRelRefs::kModifiedBy][pair.first].insert(
            std::end(relationship_by_type_table_[PKBRelRefs::kModifiedBy][pair.first]),
            std::begin(pair.second),
            std::end(pair.second)
        );
        for (auto entity_pairs : pair.second) {
          entities_in_relationship_by_types_table_[PKBRelRefs::kUsedBy][pair.first].push_back(std::get<0>(entity_pairs));
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
    } else {
      return "";
    }
}


DesignEntity PKB::EntityToDesignEntity(Entity* entity) {
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
            return DesignEntity::kInvalid;
        case (EntityEnum::kNone):
            return DesignEntity::kInvalid;
    }
}