#ifndef AUTOTESTER_DELIVERABLE_H
#define AUTOTESTER_DELIVERABLE_H

#include <unordered_map>
#include "model/Entity.h"
#include "model/Statement.h"

/**
 * This is a wrapper class for the data structures to be stored in the PKB after the SourceProcessor is done processing
 * the source file.
 */
class Deliverable {
 public:
  Deliverable() = default;    // should be initializing the data structures here
  // Adding of Procedure, Variable, ConstantValue are added using pointers to the list in EntityFactory.
  void AddStatement(Statement* stmt);

  void AddIfEntity(IfEntity* if_entity);
  void AddWhileEntity(WhileEntity* while_entity);
  void AddAssignEntity(AssignEntity* assign_entity);
  void AddCallEntity(CallEntity* call_entity);
  void AddPrintEntity(PrintEntity* print_entity);
  void AddReadEntity(ReadEntity* read_entity);

  void AddFollowRelationship(Statement* f1, Statement* f2);
  void AddParentRelationship(Statement* p1, Statement* p2);
  //TODO: Add uses / modifies hashmaps functions


  std::list<Procedure*>* GetProcList();
  std::list<Variable*>* GetVariableList();
  std::list<ConstantValue*>* GetConstantValue();

 private:
  //root-node of AST
  Program* program_;

  // EntityTables
  std::list<Procedure*> proc_list_;
  std::list<Variable*> var_list_;
  std::list<ConstantValue*> const_list_;
  std::list<Statement*> stmt_list_;
  std::list<IfEntity*> if_list_;
  std::list<WhileEntity*> while_list_;
  std::list<AssignEntity*> assign_list_;
  std::list<CallEntity*> call_list_;
  std::list<PrintEntity*> print_list_;
  std::list<ReadEntity*> read_list_;

  // RelationshipTables
  std::unordered_map<Statement*, Statement*> follow_hash_;
  std::unordered_map<Statement*, std::list<Statement*>*> parent_hash_;
  std::unordered_map<Statement*, std::list<Statement*>*> use_hash_;
  std::unordered_map<Statement*, Statement*> modifies_hash_;

};

#endif //AUTOTESTER_DELIVERABLE_H
