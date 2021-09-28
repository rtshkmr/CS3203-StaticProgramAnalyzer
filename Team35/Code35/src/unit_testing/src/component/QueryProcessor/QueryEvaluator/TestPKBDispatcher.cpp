#include "catch.hpp"
#include "component/QueryProcessor/QueryEvaluator/PKBDispatcher.h"
#include "component/PKB/PKB.h"
#include "datatype/Deliverable.h"

/* procedure Transitive {
    y = 2 * z;        //1
    if (q != 1) then    {    //2
        z = x + 24;    //3
    } else {
        while (z > 0) {    //4
            y = x * q - 5;    //5
        }
        y = x + y * z + p * q;    //6
    }
 */
//Deliverable* SetUpDeliverable_Transitive() {
//  // procedure
//  Deliverable* deliverable = new Deliverable();
//  Procedure* proc = new Procedure(new ProcedureName("Week4"));
//  Program* program = new Program(proc);
//  deliverable->SetProgram(program);
//  std::list<Procedure*>* proc_list = deliverable->GetProcList();
//  proc_list->push_back(proc);
//
//  std::list<Variable*>* var_list = deliverable->GetVariableList();
//  std::list<ConstantValue*>* const_list = deliverable->GetConstantValueList();
//
//  // y = 2 * z;
//  ConstantValue* cv_2 = new ConstantValue("2");
//  const_list->push_back(cv1);
//  Variable* var_y = new Variable(new VariableName("y"));
//  var_list->push_back(var_y);
//  Variable* var_z = new Variable(new VariableName("z"));
//  var_list->push_back(var_z);
//
//  std::vector<Variable*> stmt1_var_expr;
//  std::vector<ConstantValue*> stmt1_cv_expr;
//  stmt1_cv_expr.push_back(cv_2);
//  stmt1_var_expr.push_back(var_z);
//
//  AssignEntity* stmt1 = new AssignEntity(var_y, "y = 2 * z", stmt1_var_expr, stmt1_cv_expr);
//  stmt1->SetStatementNumber(new StatementNumber(1));
//  proc->AddStatement(stmt1);
//  deliverable->AddStatement(stmt1);
//  deliverable->AddAssignEntity(stmt1);
//  deliverable->AddModifiesRelationship(stmt3, stmt3->GetVariable());
//  deliverable->AddUsesRelationship(stmt3, stmt3->GetVariable());
//
