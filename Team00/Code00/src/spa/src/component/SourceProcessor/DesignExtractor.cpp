#include "DesignExtractor.h"

DesignExtractor::DesignExtractor(Deliverable* deliverable) {
  this->deliverable_ = deliverable;
}

/**
 * Extracts transitive relationships using the lists and tables from the deliverable.
 */
void DesignExtractor::ExtractDesignAbstractions() {
  ExtractUses();
  ExtractModifies();
  ExtractParentT(deliverable_->parent_to_child_hash_);
  ExtractFollowsT(deliverable_->follow_hash_);
  ExtractParentOfT(deliverable_->child_to_parent_hash_);
  ExtractFollowedByT(deliverable_->followed_by_hash_);
}

/**
 * Extracts transitive Uses relationship from nested containers and called procedures.
 * For e.g. there can be a while container contained in another while container.
 * A procedure can also call another procedure.
 */
void DesignExtractor::ExtractUses() {
  /*
   * for proc in proc list
   * if proc has not been extracted, use recursive helper function:
   *  for statement in stmt list
   *    if if/while/call
   *      recurse with inner stmt list
   *      add returned list of var to curr container
   *    base case no inner stmt list
   *    return list of var from curr container
   */
  std::vector<Procedure*> extracted_procedures;
  for (Procedure* proc: deliverable_->proc_list_) {
    if (std::find(extracted_procedures.begin(), extracted_procedures.end(), proc)
        == extracted_procedures.end()) { // procedure not found in extracted_procedures
      ExtractUsesInContainer(proc, &extracted_procedures);
      extracted_procedures.push_back(proc);
    }
  }
}

/**
 * Extracts transitive Modifies relationship from nested containers and called procedures.
 * For e.g. there can be a while container contained in another while container.
 * A procedure can also call another procedure.
 */
void DesignExtractor::ExtractModifies() {

}

void DesignExtractor::ExtractParentT(std::unordered_map<Statement*, std::list<Statement*>*> parent_hash) {

}

void DesignExtractor::ExtractFollowsT(std::unordered_map<Statement*, Statement*> follow_hash) {

}

void DesignExtractor::ExtractParentOfT(std::unordered_map<Statement*, Statement*> child_to_parent_hash) {

}

void DesignExtractor::ExtractFollowedByT(std::unordered_map<Statement*, Statement*> followed_by_hash) {

}

/**
 * This is a private recursive helper function that extracts Uses relationship from a Container and adds to the
 * deliverable.
 * Assumes that the variables of the direct children Statements of the container has already been added into the
 * deliverable.
 *
 * @param container Stores a list of Statements.
 * @param extracted_procedures Vector that stores the procedures that have been extracted.
 * @return A list of Variables that are found in a Container.
 */
std::list<Variable*>* DesignExtractor::ExtractUsesInContainer(Container* container,
                                                              std::vector<Procedure*>* extracted_procedures) {
  for (Statement* statement: *container->GetStatementList()) {
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
void DesignExtractor::ExtractUsesInIfContainer(IfEntity* if_entity,
                                               Container* container,
                                               std::vector<Procedure*>* extracted_procedures) {
  std::list<Variable*>* nested_if_var_list = ExtractUsesInContainer(if_entity, extracted_procedures);
  deliverable_->AddUsesRelationship(container, nested_if_var_list);

  std::list<Variable*>* nested_else_var_list = ExtractUsesInContainer(if_entity->GetElseEntity(), extracted_procedures);
  deliverable_->AddUsesRelationship(container, nested_else_var_list);
}

/**
 * Helper function to handle while condition in ExtractUsesInContainer.
 */
void DesignExtractor::ExtractUsesInWhileContainer(WhileEntity* while_entity,
                                                  Container* container,
                                                  std::vector<Procedure*>* extracted_procedures) {
  std::list<Variable*>* nested_var_list = ExtractUsesInContainer(while_entity, extracted_procedures);
  deliverable_->AddUsesRelationship(container, nested_var_list);
}

/**
 * Helper function to handle call condition in ExtractUsesInContainer.
 */
void DesignExtractor::ExtractUsesInCallContainer(CallEntity* call_entity,
                                                 Container* container,
                                                 std::vector<Procedure*>* extracted_procedures) {
  Procedure* called_proc = call_entity->getProcedure();
  std::list<Variable*>* var_list;
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
}
