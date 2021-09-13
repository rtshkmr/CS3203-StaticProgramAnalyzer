#include <cassert>
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
  ExtractParentTRelationship();
  ExtractFollowsTRelationship();
}

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
void DesignExtractor::ExtractUses() {
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
  // same algorithm as ExtractUses
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
 * Extracts Parent* recursively for each procedure.
 *
 * pseudocode:
 * for each proc in proc list
 *  for each stmt in stmt list
 *    recurse helper(stmt):
 *      get children list
 *      base case: no children list, return empty list
 *      for each child
 *        recurse(child) and get childrenT list
 *        add this child and its childrenT list to parentT
 *      return all childrenT of this stmt
 */
void DesignExtractor::ExtractParentTRelationship() {
  for (Procedure* proc: deliverable_->proc_list_) {
    for (Statement* stmt: *proc->GetStatementList()) {
      ExtractChildrenTFromParent(stmt);
    }
  }
}

/**
 * Extracts ChildrenT recursively from a parent Statement. Adds all intermediate ParentT relationship to the deliverable
 * and returns the ChildrenT list.
 *
 * @param parent Statement that may have children.
 * @return List of Statements that are the childrenT of parent.
 */
std::list<Statement*>* DesignExtractor::ExtractChildrenTFromParent(Statement* parent) {
  std::unordered_map<Statement*, std::list<Statement*>*> ptc = deliverable_->parent_to_child_hash_;

  // base case: children list not found
  if (ptc.find(parent) == ptc.end()) {
    // does not return itself bcos this function is meant to return its childrenT
    return new std::list<Statement*>();
  }

  std::list<Statement*>* children = ptc.find(parent)->second;
  for (Statement* child: *children) {
    std::list<Statement*>* children_T_list = ExtractChildrenTFromParent(child);
    // add this child to the parent
    deliverable_->AddParentTransitiveRelationship(parent, child);
    // add this child's childrenT to the parent
    deliverable_->AddParentTransitiveRelationshipForList(parent, children_T_list);
  }

  std::unordered_map<Statement*, std::list<Statement*>*> ptct = deliverable_->parent_to_child_hash_;
  assert(ptct.find(parent) != ptct.end());  // there must be some ParentT bcos there is a children list
  // return all childrenT
  return ptct.find(parent)->second;
}

/**
 * Extracts FollowsT from each procedure.
 *
 * pseudocode:
 * for each proc in proc list
 *  recurse using the first stmt in stmt list:
 *    if stmt is container
 *      recurse on the first stmt in its stmt list
 *      // this starts a new thread of extracting
 *    // continue
 *    if stmt found in follows hash
 *      recurse(following stmt) and get followsT of following stmt
 *      add the following stmt and followsT to followsT hash of stmt
 *    else base case: no following statement
 *      return empty list
 */
void DesignExtractor::ExtractFollowsTRelationship() {
  for (Procedure* proc: deliverable_->proc_list_) {
    Statement* first_statement = proc->GetStatementList()->front();
    ExtractFollowsTFromThread(first_statement);
  }
}
/**
 * Extracts Follows* relationship recursively from the thread of Statements that Follow one another transitively.
 * Adds any intermediate Follows* relationship to the deliverable.
 *
 * @param top The Statement at the top of the Follows* thread.
 * @return List of Statements that Follows* the top statement.
 */
std::list<Statement*>* DesignExtractor::ExtractFollowsTFromThread(Statement* top) {
  // Check if top statement is a container, i.e. if/while
  if (Container* container = dynamic_cast<Container*>(top)) {
    Statement* first_statement = container->GetStatementList()->front();
    ExtractFollowsTFromThread(first_statement);
    // Statements in the container will not follow the container
    // , so there is no need to collect its result.
  }

  std::unordered_map<Statement*, Statement*> follows_hash = deliverable_->follow_hash_;
  if (follows_hash.find(top) != follows_hash.end()) {
    // there is a statement following top
    Statement* following_statement = follows_hash.find(top)->second;
    std::list<Statement*>* follows_T_list = ExtractFollowsTFromThread(following_statement);
    deliverable_->AddFollowsTransitiveRelationship(top, following_statement);
    deliverable_->AddFollowsTransitiveRelationshipForList(top, follows_T_list);

    std::unordered_map<Statement*, std::list<Statement*>*> ft = deliverable_->follows_T_hash_;
    assert(ft.find(top) != ft.end()); // there must be some FollowsT bcos there is Follows
    // returns all FollowsT of top
    return ft.find(top)->second;
  } else {
    // top is last statement in thread
    return new std::list<Statement*>();
  }
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
  // Variables in Else container may be added to the deliverable, but must also be added to the If container entry of
  // the map in deliverable
  // Then the nested Variables in If can be extracted and added to the outer container
  std::list<Variable*>* nested_else_var_list = ExtractUsesInContainer(if_entity->GetElseEntity(), extracted_procedures);
  deliverable_->AddUsesRelationship(container, nested_else_var_list);
  deliverable_->AddUsesRelationship(if_entity, nested_else_var_list);

  std::list<Variable*>* nested_if_var_list = ExtractUsesInContainer(if_entity, extracted_procedures);
  deliverable_->AddUsesRelationship(container, nested_if_var_list);

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
std::list<Variable*>* DesignExtractor::ExtractModifiesInContainer(Container* container,
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
void DesignExtractor::ExtractModifiesInIfContainer(IfEntity* if_entity,
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
void DesignExtractor::ExtractModifiesInWhileContainer(WhileEntity* while_entity,
                                                      Container* container,
                                                      std::vector<Procedure*>* extracted_procedures) {
  std::list<Variable*>* nested_var_list = ExtractModifiesInContainer(while_entity, extracted_procedures);
  deliverable_->AddModifiesRelationship(container, nested_var_list);
}

/**
 * Helper function to handle call condition in ExtractModifiesInContainer.
 */
void DesignExtractor::ExtractModifiesInCallContainer(CallEntity* call_entity,
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
