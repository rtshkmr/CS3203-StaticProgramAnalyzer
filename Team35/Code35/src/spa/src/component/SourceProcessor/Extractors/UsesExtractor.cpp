#include "UsesExtractor.h"

/**
 * Extracts transitive Uses relationship from nested containers and called procedures.
 * For e.g. there can be a while container contained in another while container.
 * A procedure can also call another procedure.
 *
 * pseudocode:
 * for proc in proc list
 * if proc has not been extracted, use recursive helper function:
 *  for statement in stmt list
 *    if if/while/call
 *      recurse with inner stmt list
 *      add returned list of var to curr container
 *    else: base case: no inner stmt list
 *    return list of var from curr container
 */
void UsesExtractor::Extract(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

  std::vector<Procedure*> extracted_procedures;
  for (Procedure* proc: deliverable_->proc_list_) {
    if (std::find(extracted_procedures.begin(), extracted_procedures.end(), proc)
        == extracted_procedures.end()) { // procedure not found in extracted_procedures
      ExtractUsesInContainer(proc, & extracted_procedures);
      extracted_procedures.push_back(proc);
    }
  }

  EraseElseFromUses();
}

/**
 * This is a private recursive helper function that extracts Uses relationship from a Container and adds to the
 * deliverable.
 * Assumes that the variables of the direct children Statements of the container has already been added into the
 * deliverable.
 * Assumes that all local UsesP has been added by PSubsystem.
 *
 * @param container Stores a list of Statements.
 * @param extracted_procedures Vector that stores the procedures that have been extracted.
 * @return A list of Variables that are found in a Container.
 */
std::list<Variable*>* UsesExtractor::ExtractUsesInContainer(Container* container,
                                                            std::vector<Procedure*>* extracted_procedures) {
  for (Statement* statement: * container->GetStatementList()) {
    if (IfEntity* if_entity = dynamic_cast<IfEntity*>(statement)) {
      ExtractUsesInIfContainer(if_entity, container, extracted_procedures);
    } else if (WhileEntity* while_entity = dynamic_cast<WhileEntity*>(statement)) {
      ExtractUsesInWhileContainer(while_entity, container, extracted_procedures);
    } else if (CallEntity* call_entity = dynamic_cast<CallEntity*>(statement)) {
      ExtractUsesInCallContainer(call_entity, container, extracted_procedures);
    }
  }

  std::unordered_map<Container*, std::list<Variable*>*> cuh = deliverable_->container_use_hash_;
  if (cuh.find(container) != cuh.end()) {
    // container found
    return cuh.find(container)->second;
  } else {
    return new std::list<Variable*>();
  }
}

/**
 * Helper function to handle if condition in ExtractUsesInContainer.
 */
void UsesExtractor::ExtractUsesInIfContainer(IfEntity* if_entity,
                                             Container* container,
                                             std::vector<Procedure*>* extracted_procedures) {
  // Variables in Else container may be added to the deliverable, but must also be added to the If container entry of
  // the map in deliverable
  // Then the nested Variables in If can be extracted and added to the outer container
  std::list<Variable*>* nested_else_var_list = ExtractUsesInContainer(if_entity->GetElseEntity(), extracted_procedures);
  deliverable_->AddUsesRelationship(if_entity, nested_else_var_list);

  std::list<Variable*>* nested_if_var_list = ExtractUsesInContainer(if_entity, extracted_procedures);

  // todo: use this when Calls relationship table is set up
  // no need to add relationship to procedure as it is assumed to be done in PSubsystem
//  if (Procedure* proc = dynamic_cast<Procedure*>(container)) {
//    return;
//  }

  deliverable_->AddUsesRelationship(container, nested_if_var_list);
}

/**
 * Helper function to handle while condition in ExtractUsesInContainer.
 */
void UsesExtractor::ExtractUsesInWhileContainer(WhileEntity* while_entity,
                                                Container* container,
                                                std::vector<Procedure*>* extracted_procedures) {
  std::list<Variable*>* nested_var_list = ExtractUsesInContainer(while_entity, extracted_procedures);

  // todo: use this when Calls relationship table is set up
  // no need to add relationship to procedure as it is assumed to be done in PSubsystem
//  if (Procedure* proc = dynamic_cast<Procedure*>(container)) {
//    return;
//  }

  deliverable_->AddUsesRelationship(container, nested_var_list);
}

/**
 * Helper function to handle call condition in ExtractUsesInContainer.
 */
void UsesExtractor::ExtractUsesInCallContainer(CallEntity* call_entity,
                                               Container* container,
                                               std::vector<Procedure*>* extracted_procedures) {
  Procedure* called_proc = call_entity->GetProcedure();
  std::list<Variable*>* var_list;

  // extracted_procedures is needed to ensure dfs of call stmts. When a called_proc has not been extracted, it might
  // have call stmts whose transitive relations within the call stmts are not yet extracted so the dfs must extract from
  // the called_proc before continuing with the current procedure.
  if (std::find(extracted_procedures->begin(), extracted_procedures->end(), called_proc)
      != extracted_procedures->end()) { // procedure found in extracted_procedures

    std::unordered_map<Container*, std::list<Variable*>*> cuh = deliverable_->container_use_hash_;
    if (cuh.find(called_proc) != cuh.end()) {
      // container found in container_use_hash_ in deliverable
      var_list = cuh.find(called_proc)->second;
    } else {
      var_list = new std::list<Variable*>();
    }

  } else {
    var_list = ExtractUsesInContainer(called_proc, extracted_procedures);
    extracted_procedures->push_back(called_proc);
  }

  deliverable_->AddUsesRelationship(container, var_list);
  for (Variable* var: * var_list) {
    deliverable_->AddUsesRelationship(call_entity, var);
  }
  // todo: use this when Calls relationship table is set up and the current_procedure of a call stmt can be found
  // if this call stmt is nested, add the relationships inside this call stmt to the current procedure too, because
  // 1st level containers of procedures will not add to the procedure.
//  if (dynamic_cast<Procedure*>(container) == nullptr) {
//    deliverable_->AddUsesRelationship(current_procedure, var_list);
//  }
}

/**
 * Erases else entries in the container_uses_hash and container_used_by_hash. Else entries are created as an
 * intermediate result when there is nesting within the Else block, and are not needed for queries.
 */
void UsesExtractor::EraseElseFromUses() {
  std::list<Container*> deleting_cont_list;
  for (auto pair: deliverable_->container_use_hash_) {
    Container* container = pair.first;
    std::list<Variable*>* var_list = pair.second;
    if (dynamic_cast<ElseEntity*>(container)) {
      EraseReverseRelationship(container, var_list);
      deleting_cont_list.push_back(container);
    }
  }
  for (auto cont: deleting_cont_list) {
    deliverable_->container_use_hash_.erase(cont);
  }
}

void UsesExtractor::EraseReverseRelationship(Container* container, std::list<Variable*>* var_list) {
  for (auto var: * var_list) {
    if (deliverable_->container_used_by_hash_.count(var)) {
      std::list<Container*>* var_cont_list = deliverable_->container_used_by_hash_.find(var)->second;
      var_cont_list->remove(container);
      if (var_cont_list->empty()) {
        deliverable_->container_used_by_hash_.erase(var);
      }
    }
  }
}
