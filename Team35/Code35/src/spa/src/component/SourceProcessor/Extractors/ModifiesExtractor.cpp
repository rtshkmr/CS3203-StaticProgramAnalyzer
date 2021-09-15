#include "ModifiesExtractor.h"

/**
 * Extracts transitive Modifies relationship from nested containers and called procedures.
 * For e.g. there can be a while container contained in another while container.
 * A procedure can also call another procedure.
 */
void ModifiesExtractor::Extract(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

  // same algorithm as Extract
  std::vector<Procedure*> extracted_procedures;
  for (Procedure* proc: deliverable_->proc_list_) {
    if (std::find(extracted_procedures.begin(), extracted_procedures.end(), proc)
    == extracted_procedures.end()) { // procedure not found in extracted_procedures
      ExtractModifiesInContainer(proc, &extracted_procedures);
      extracted_procedures.push_back(proc);
    }
  }
}

/**
 * This is a private recursive helper function that extracts Modifies relationship from a Container and adds to the
 * deliverable.
 * Assumes that the variables of the direct children Statements of the container has already been added into the
 * deliverable.
 *
 * @param container Stores a list of Statements.
 * @param extracted_procedures Vector that stores the procedures that have been extracted.
 * @return A list of Variables that are found in a Container.
 */
std::list<Variable*>* ModifiesExtractor::ExtractModifiesInContainer(Container* container,
                                                                  std::vector<Procedure*>* extracted_procedures) {
  for (Statement* statement: *container->GetStatementList()) {
    if (IfEntity* if_entity = dynamic_cast<IfEntity*>(statement)) {
      ExtractModifiesInIfContainer(if_entity, container, extracted_procedures);
    } else if (WhileEntity* while_entity = dynamic_cast<WhileEntity*>(statement)) {
      ExtractModifiesInWhileContainer(while_entity, container, extracted_procedures);
    } else if (CallEntity* call_entity = dynamic_cast<CallEntity*>(statement)) {
      ExtractModifiesInCallContainer(call_entity, container, extracted_procedures);
    }
  }

  std::unordered_map<Container*, std::list<Variable*>*> cmh = deliverable_->container_modifies_hash_;
  if (cmh.find(container) != cmh.end()) {
    // container found
    return cmh.find(container)->second;
  } else {
    return new std::list<Variable*>();
  }
}

/**
 * Helper function to handle if condition in ExtractModifiesInContainer.
 */
void ModifiesExtractor::ExtractModifiesInIfContainer(IfEntity* if_entity,
                                                   Container* container,
                                                   std::vector<Procedure*>* extracted_procedures) {
  // Variables in Else container may be added to the deliverable, but must also be added to the If container entry of
  // the map in deliverable
  // Then the nested Variables in If can be extracted and added to the outer container
  std::list<Variable*>* nested_else_var_list
  = ExtractModifiesInContainer(if_entity->GetElseEntity(), extracted_procedures);
  deliverable_->AddModifiesRelationship(container, nested_else_var_list);
  deliverable_->AddModifiesRelationship(if_entity, nested_else_var_list);

  std::list<Variable*>* nested_if_var_list = ExtractModifiesInContainer(if_entity, extracted_procedures);
  deliverable_->AddModifiesRelationship(container, nested_if_var_list);

}

/**
 * Helper function to handle while condition in ExtractModifiesInContainer.
 */
void ModifiesExtractor::ExtractModifiesInWhileContainer(WhileEntity* while_entity,
                                                      Container* container,
                                                      std::vector<Procedure*>* extracted_procedures) {
  std::list<Variable*>* nested_var_list = ExtractModifiesInContainer(while_entity, extracted_procedures);
  deliverable_->AddModifiesRelationship(container, nested_var_list);
}

/**
 * Helper function to handle call condition in ExtractModifiesInContainer.
 */
void ModifiesExtractor::ExtractModifiesInCallContainer(CallEntity* call_entity,
                                                     Container* container,
                                                     std::vector<Procedure*>* extracted_procedures) {
  Procedure* called_proc = call_entity->getProcedure();
  std::list<Variable*>* var_list;
  if (std::find(extracted_procedures->begin(), extracted_procedures->end(), called_proc)
  != extracted_procedures->end()) { // procedure found in extracted_procedures

    std::unordered_map<Container*, std::list<Variable*>*> cmh = deliverable_->container_modifies_hash_;
    if (cmh.find(called_proc) != cmh.end()) {
      // container found in container_use_hash_ in deliverable
      var_list = cmh.find(called_proc)->second;
    } else {
      var_list = new std::list<Variable*>();
    }

  } else {
    var_list = ExtractModifiesInContainer(called_proc, extracted_procedures);
    extracted_procedures->push_back(called_proc);
  }
  deliverable_->AddModifiesRelationship(container, var_list);
}