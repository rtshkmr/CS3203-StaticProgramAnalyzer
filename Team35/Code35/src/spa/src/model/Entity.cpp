/**
 * This file contains the implementation for Entity object Entity, Procedure, Variable, Program.
 */

#include <list>
#include "Entity.h"

EntityEnum Entity::GetEntityEnum() {
  return type;
}

void Container::AddStatement(Statement* stmt) {
  statement_list_.push_back(stmt);
}

std::list<Statement*>* Container::GetStatementList() {
  return &statement_list_;
}

Procedure::Procedure(ProcedureName* pName) {
  type = EntityEnum::kProcedureEntity;
  procedure_name_ = pName;
}

const ProcedureName* Procedure::GetName() {
  return procedure_name_;
}

const void Procedure::SetClusterRoot(Cluster* cluster) {
  this->cluster_root_ = cluster;
}
const Cluster* Procedure::GetClusterRoot() {
  return this->cluster_root_;
}
const Block* Procedure::GetBlockRoot() {
  return this->block_root_;
}
const void Procedure::SetBlockRoot(Block* block_root) {
  this->block_root_ = block_root;
}

const void Procedure::SetBlockTail(Block* block_tail) {
  this->block_tail_ = block_tail;
}

Variable::Variable(VariableName* vName) {
  type = EntityEnum::kNone;
  variable_name_ = vName;
}

const VariableName* Variable::GetName() {
  return variable_name_;
}

/**
* This function contains the algorithm to sort a vector<Variable*>.
* @param var_list The vector<Variable*> to be sorted.
* @return The vector<Variable*> after sorting.
*/
std::vector<Variable*> Variable::SortVariableVector(std::vector<Variable*> var_list) {
  std::vector<Variable*> var_list_copy = var_list;
  std::sort(var_list_copy.begin(), var_list_copy.end(),
            [](Variable* a, Variable* b) { return *a->GetName() < *b->GetName(); });
  return var_list_copy;
}

Program::Program(Procedure* p) {
  procedure_list_.push_back(p);
}

std::list<Procedure*>* Program::GetProcedureList() {
  return & procedure_list_;
}

void Program::AddProcedure(Procedure* p) {
  procedure_list_.push_back(p);
}

void Statement::SetStatementNumber(StatementNumber* sn) {
  statement_number_ = sn;
}

void Statement::SetLineNumber(LineNumber* ln) {
  line_number_ = ln;
}

void Statement::SetParentNode(Container* parent) {
  parent_node_ = parent;
}

void Statement::SetBeforeNode(Statement* before) {
  before_node_ = before;
}

Container* Statement::GetParentNode() {
  return parent_node_;
}

Statement* Statement::GetBeforeNode() {
  return before_node_;
}

StatementNumber* Statement::GetStatementNumber() {
  return statement_number_;
}

LineNumber* Statement::GetLineNumber() {
  return line_number_;
}
