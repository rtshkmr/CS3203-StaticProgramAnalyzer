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
 public: //TODO: create getters before turning private
  Deliverable() = default;
  // Adding of Procedure, Variable, ConstantValue are added using pointers to the list in EntityFactory.
  void AddStatement(Statement* stmt);

  void AddIfEntity(IfEntity* if_entity);
  void AddWhileEntity(WhileEntity* while_entity);
  void AddAssignEntity(AssignEntity* assign_entity);
  void AddCallEntity(CallEntity* call_entity);
  void AddPrintEntity(PrintEntity* print_entity);
  void AddReadEntity(ReadEntity* read_entity);

  void AddFollowRelationship(Statement* f1, Statement* f2);
  void AddFollowsTransitiveRelationship(Statement* before, Statement* after);
  void AddFollowsTransitiveRelationshipForList(Statement* before, std::list<Statement*>* afters);
  void AddParentRelationship(Statement* parent, Statement* child);
  void AddParentTransitiveRelationship(Statement* parent, Statement* child);
  void AddParentTransitiveRelationshipForList(Statement* parent, std::list<Statement*>* children);
  void AddUsesRelationship(Statement* u1, Variable* u2);
  void AddUsesRelationship(Container* u1, Variable* u2);
  void AddUsesRelationship(Container* container, std::list<Variable*>* var_list);
  void AddModifiesRelationship(Statement* m1, Variable* m2);
  void AddModifiesRelationship(Container* m1, Variable* m2);
  void AddModifiesRelationship(Container* container, std::list<Variable*>* var_list);
  void AddCallsRelationship(Procedure* p1, Procedure* p2);
  void AddCallsTransitiveRelationship(Procedure* p1, Procedure* p2);
  void AddCallsTransitiveRelationshipForList(Procedure* p1, std::list<Procedure*>* proc_list);
  void AddNextRelationship(Statement* s1, Statement* s2);

  void SetProgram(Program* program);
  Program* GetProgram();
  std::list<Procedure*>* GetProcList();
  std::list<Variable*>* GetVariableList();
  std::list<ConstantValue*>* GetConstantValueList();
  std::vector<Statement*>* GetStatementList();
  std::list<IfEntity*>* GetIfList();
  std::list<WhileEntity*>* GetWhileList();
  std::list<AssignEntity*>* GetAssignList();
  std::list<CallEntity*>* GetCallList();
  std::list<PrintEntity*>* GetPrintList();
  std::list<ReadEntity*>* GetReadList();

  //root-node of AST
  Program* program_;

  // EntityTables
  std::list<Procedure*> proc_list_;
  std::list<Variable*> var_list_;
  std::list<ConstantValue*> const_list_;
  std::vector<Statement*> stmt_list_;
  std::list<IfEntity*> if_list_;
  std::list<WhileEntity*> while_list_;
  std::list<AssignEntity*> assign_list_;
  std::list<CallEntity*> call_list_;
  std::list<PrintEntity*> print_list_;
  std::list<ReadEntity*> read_list_;

  // RelationshipTables
  std::unordered_map<Statement*, Statement*> follow_hash_;
  std::unordered_map<Statement*, std::list<Statement*>*> follows_T_hash_; // to store Follows*
  std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_hash_;
  std::unordered_map<Statement*, std::list<Statement*>*> parent_to_child_T_hash_; // to store Parent*
  std::unordered_map<Statement*, std::list<Variable*>*> use_hash_; //to store Uses(x,_), where x is stmt
  std::unordered_map<Container*, std::list<Variable*>*>
      container_use_hash_; //to store Uses(x,_), where x is if, while, procedure
  std::unordered_map<Statement*, std::list<Variable*>*> modifies_hash_; //to store Modifies(x,_), where x is stmt
  std::unordered_map<Container*, std::list<Variable*>*>
      container_modifies_hash_; //to store Modifies(x,_), where x is if, while, procedure
  std::unordered_map<Procedure*, std::list<Procedure*>*> calls_hash_; // represents a Calls(x, y) where x -> y
  std::unordered_map<Procedure*, std::list<Procedure*>*> calls_T_hash_; // represents a Calls*(x, y) where y -> x
  std::unordered_map<Statement*, std::list<Statement*>*> next_hash_;

  //Relationship (Reverse) Tables
  std::unordered_map<Statement*, Statement*> followed_by_hash_;
  std::unordered_map<Statement*, std::list<Statement*>*> followed_by_T_hash_; // to store FollowedBy*
  std::unordered_map<Statement*, Statement*> child_to_parent_hash_;
  std::unordered_map<Statement*, std::list<Statement*>*> child_to_parent_T_hash_; // to store Child*
  std::unordered_map<Variable*, std::list<Statement*>*> used_by_hash_; //to store Uses(x,_), where x is stmt
  std::unordered_map<Variable*, std::list<Container*>*>
      container_used_by_hash_; //to store Uses(x,_), where x is if, while, procedure
  std::unordered_map<Variable*, std::list<Statement*>*> modified_by_hash_; //to store Modifies(x,_), where x is stmt
  std::unordered_map<Variable*, std::list<Container*>*>
      container_modified_by_hash_; //to store Modifies(x,_), where x is if, while, procedure
  std::unordered_map<Procedure*, std::list<Procedure*>*> called_by_hash_; // represents a Calls(x, y) where y -> x
  std::unordered_map<Procedure*, std::list<Procedure*>*> called_by_T_hash_; // represents a Calls*(x, y) where y -> x
  std::unordered_map<Statement*, std::list<Statement*>*> previous_hash_;  // reverse of next

 private:
  template<typename X, typename Y>
  void AddRelationshipToMap(std::unordered_map<X, std::list<Y>*>* map, X key, Y value);
};

#endif //AUTOTESTER_DELIVERABLE_H
