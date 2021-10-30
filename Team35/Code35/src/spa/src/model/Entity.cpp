/**
 * This file contains the implementation for Entity object Entity, Procedure, Variable, Program.
 */

#include <list>
#include "Entity.h"
#include "CFG.h"

EntityEnum Entity::GetEntityEnum() {
  return type;
}

void Container::AddStatement(Statement* stmt) {
  statement_list_.push_back(stmt);
}

std::list<Statement*>* Container::GetStatementList() {
  return & statement_list_;
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
/**
 *  For a given program, returns the innermost possible cluster that contains both first and second stmts in its range.
 *  This function is written in the continuous passing style so the previous_cluster is passed for function calls.
 * @return nullptr if no such cluster exists
 * @param first_stmt
 * @param second_stmt
 * @param prev_cluster is a previous cluster that successfully encapsulates both the statements
 * @return
 */
Cluster* Procedure::GetInnermostCluster(int first_stmt, int second_stmt, Cluster* prev_cluster) {
  const Cluster* current_cluster = prev_cluster ? prev_cluster : this -> GetClusterRoot();
  if(!current_cluster->CheckIfStatementsInRange(first_stmt, second_stmt)) {
    return nullptr; // invariant: either prev cluster is null_ptr or it is a cluster that is guaranteed to contain these lines
  } else { // look into nested children
    auto nested_clusters = current_cluster ->GetNestedClusters();
    if(nested_clusters.empty()) {
      return prev_cluster;
    } else {
      auto cluster_iter = nested_clusters.begin();
      while (!(*cluster_iter)->CheckIfStmtNumInRange(first_stmt)) {
        std::advance(cluster_iter, 1);
      }
      Cluster* cluster_containing_start_stmt = *cluster_iter;
      if(cluster_containing_start_stmt->CheckIfStatementsInRange(first_stmt, second_stmt)) {
        return GetInnermostCluster(first_stmt, second_stmt, cluster_containing_start_stmt); // recurse into it
      } else {
        return const_cast<Cluster*>(current_cluster);
      }
    }
  }
}

Variable::Variable(VariableName* vName) {
  type = EntityEnum::kVariableEntity;
  variable_name_ = vName;
}

const VariableName* Variable::GetVariableName() {
  return variable_name_;
}

void Variable::AddStatement(Statement* stmt) {
  EntityEnum ent = stmt->GetEntityEnum();
  int lot = static_cast<int>(ent) - 1; // remove procedure from 0.
  var_to_statement.at(lot).insert(stmt);
}

std::vector<std::set<Statement*>> Variable::GetStatementTable() {
  return var_to_statement;
}

/**
* This function contains the algorithm to sort a vector<Variable*>.
* @param var_list The vector<Variable*> to be sorted.
* @return The vector<Variable*> after sorting.
*/
std::vector<Variable*> Variable::SortVariableVector(std::vector<Variable*> var_list) {
  std::vector<Variable*> var_list_copy = var_list;
  std::sort(var_list_copy.begin(), var_list_copy.end(),
            [](Variable* a, Variable* b) { return * a->GetVariableName() < * b->GetVariableName(); });
  return var_list_copy;
}
const std::string Variable::GetName() const{
  return variable_name_->GetName();
}

Constant::Constant(ConstantValue* cv) {
  type = EntityEnum::kConstantEntity;
  constant_value_ = cv;
}

const ConstantValue* Constant::GetValue() {
  return constant_value_;
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

/**
 * Given a target line number, returns the procedure that that line number belongs to
 * @param line_num
 * @return
 */
Procedure* Program::GetProcForLineNum(int line_num) {
  std::list<Procedure*>* proc_list = this->GetProcedureList();
  for (Procedure* proc: * proc_list) {  // todo: optimise finding procedure of target stmt
    auto* cluster_root = const_cast<Cluster*>(proc->GetClusterRoot());
    if(cluster_root->CheckIfStatementInRange(line_num)) {
      return proc;
    }
  }
  return nullptr;
}



/**
 * Given a target line number, returns the procedure cluster that contains this line number
 * @param line_num
 * @return
 */
Cluster* Program::GetProcClusterForLineNum(int line_num) {
    Procedure* proc = this->GetProcForLineNum(line_num);
    if(proc) {
      return const_cast<Cluster*>(proc->GetClusterRoot());
    } else {
      return nullptr;
    }
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
std::string Statement::GetLineNumberString() {
  return std::to_string(line_number_->getNum());
}
