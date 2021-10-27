#include "VariableTExtractor.h"

VariableTExtractor::VariableTExtractor(Deliverable* deliverable) {
  deliverable_ = deliverable;
}

void VariableTExtractor::Extract(VariableRel rel_type) {
  rel_type_ = rel_type;
  if (rel_type == VariableRel::kModifies) {
    rel_map_ = &deliverable_->container_modifies_hash_;
    reverse_rel_map_ = &deliverable_->container_modified_by_hash_;
  } else if (rel_type == VariableRel::kUses) {
    rel_map_ = &deliverable_->container_use_hash_;
    reverse_rel_map_ = &deliverable_->container_used_by_hash_;
  } else {
    return;
  }

  std::vector<Procedure*> extracted_procedures;
  for (Procedure* proc: deliverable_->proc_list_) {
    if (std::find(extracted_procedures.begin(), extracted_procedures.end(), proc)
        == extracted_procedures.end()) { // procedure not found in extracted_procedures
      ExtractFromContainer(proc, &extracted_procedures);
      extracted_procedures.push_back(proc);
    }
  }
  EraseElse();
}

std::list<Variable*>* VariableTExtractor::ExtractFromContainer(Container* container,
                                                               std::vector<Procedure*>* extracted_procedures) {
  for (Statement* statement: *container->GetStatementList()) {
    if (IfEntity* if_entity = dynamic_cast<IfEntity*>(statement)) {
      ExtractFromIfContainer(if_entity, container, extracted_procedures);
    } else if (WhileEntity* while_entity = dynamic_cast<WhileEntity*>(statement)) {
      ExtractFromWhileContainer(while_entity, container, extracted_procedures);
    } else if (CallEntity* call_entity = dynamic_cast<CallEntity*>(statement)) {
      ExtractFromCallContainer(call_entity, container, extracted_procedures);
    }
  }

  if (rel_map_->find(container) != rel_map_->end()) { // container found
    return rel_map_->find(container)->second;
  } else {
    return new std::list<Variable*>();
  }
}

void VariableTExtractor::ExtractFromIfContainer(IfEntity* if_entity,
                                                Container* container,
                                                std::vector<Procedure*>* extracted_procedures) {
  // Variables in Else container may be added to the deliverable, but must also be added to the If container entry of
  // the map in deliverable
  // Then the nested Variables in If can be extracted and added to the outer container
  std::list<Variable*>* nested_else_var_list
      = ExtractFromContainer(if_entity->GetElseEntity(), extracted_procedures);
  AddRelationship(if_entity, nested_else_var_list);

  std::list<Variable*>* nested_if_var_list = ExtractFromContainer(if_entity, extracted_procedures);
  AddRelationship(container, nested_if_var_list);
}

void VariableTExtractor::ExtractFromWhileContainer(WhileEntity* while_entity,
                                                   Container* container,
                                                   std::vector<Procedure*>* extracted_procedures) {
  std::list<Variable*>* nested_var_list = ExtractFromContainer(while_entity, extracted_procedures);
  AddRelationship(container, nested_var_list);
}

void VariableTExtractor::ExtractFromCallContainer(CallEntity* call_entity,
                                                  Container* container,
                                                  std::vector<Procedure*>* extracted_procedures) {
  Procedure* called_proc = call_entity->GetProcedure();
  std::list<Variable*>* var_list;

  // extracted_procedures is needed to ensure dfs of call stmts. When a called_proc has not been extracted, it might
  // have call stmts whose transitive relations within the call stmts are not yet extracted so the dfs must extract from
  // the called_proc before continuing with the current procedure.
  if (std::find(extracted_procedures->begin(), extracted_procedures->end(), called_proc)
      != extracted_procedures->end()) { // procedure found in extracted_procedures
    if (rel_map_->find(called_proc) != rel_map_->end()) { // container found
      var_list = rel_map_->find(called_proc)->second;
    } else {
      var_list = new std::list<Variable*>();
    }
  } else {
    var_list = ExtractFromContainer(called_proc, extracted_procedures);
    extracted_procedures->push_back(called_proc);
  }

  AddRelationship(container, var_list);
  for (Variable* var: *var_list) {
    AddRelationship(call_entity, var);
  }
}

void VariableTExtractor::AddRelationship(Container* container, std::list<Variable*>* var_list) {
  if (rel_type_ == VariableRel::kModifies) {
    deliverable_->AddModifiesRelationship(container, var_list);
  } else {  // uses
    deliverable_->AddUsesRelationship(container, var_list);
  }
}

void VariableTExtractor::AddRelationship(Statement* statement, Variable* var) {
  if (rel_type_ == VariableRel::kModifies) {
    deliverable_->AddModifiesRelationship(statement, var);
  } else {  // uses
    deliverable_->AddUsesRelationship(statement, var);
  }
}

void VariableTExtractor::EraseElse() {
  std::list<Container*> deleting_cont_list;
  for (auto pair: *rel_map_) {
    Container* container = pair.first;
    std::list<Variable*>* var_list = pair.second;
    if (dynamic_cast<ElseEntity*>(container)) {
      EraseReverseRelationship(container, var_list);
      deleting_cont_list.push_back(container);
    }
  }
  for (auto cont: deleting_cont_list) {
    rel_map_->erase(cont);
  }
}

void VariableTExtractor::EraseReverseRelationship(Container* container, std::list<Variable*>* var_list) {
  for (auto var: *var_list) {
    if (reverse_rel_map_->count(var)) {
      std::list<Container*>* var_cont_list = reverse_rel_map_->find(var)->second;
      var_cont_list->remove(container);
      if (var_cont_list->empty()) {
        reverse_rel_map_->erase(var);
      }
    }
  }
}
