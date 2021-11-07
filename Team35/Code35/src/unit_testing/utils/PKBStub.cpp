//
// Created by Max Ng on 3/10/21.
//

#include "PKBStub.h"

/* procedure Week4 {
 * read x;
 * print x;
 * y = 1;
 * z = 3;
 * z = x + y + 3;
 * }
 */
Deliverable* SetUpDeliverable_Week4() {
  /// Using ./tests/basic_source.txt as input

  // procedure
  Deliverable* deliverable = new Deliverable();
  Procedure* proc = new Procedure(new ProcedureName("Week4"));
  Program* program = new Program(proc);
  deliverable->SetProgram(program);
  std::list<Procedure*>* proc_list = deliverable->GetProcList();
  proc_list->push_back(proc);

  std::list<Variable*>* var_list = deliverable->GetVariableList();
  std::list<Constant*>* const_list = deliverable->GetConstantList();

  // read x
  Variable* var_x = new Variable(new VariableName("x"));
  var_list->push_back(var_x);

  ReadEntity* stmt1 = new ReadEntity(var_x);
  stmt1->SetStatementNumber(new StatementNumber(1));
  proc->AddStatement(stmt1);
  deliverable->AddStatement(stmt1);
  deliverable->AddReadEntity(stmt1);
  deliverable->AddModifiesRelationship(stmt1, stmt1->GetVariableObj());

  // print x
  PrintEntity* stmt2 = new PrintEntity(var_x);
  stmt2->SetStatementNumber(new StatementNumber(2));
  stmt2->SetBeforeNode(stmt1);
  proc->AddStatement(stmt2);
  deliverable->AddStatement(stmt2);
  deliverable->AddPrintEntity(stmt2);
  deliverable->AddUsesRelationship(stmt2, stmt2->GetVariableObj());

  // y = 1
  Constant* cv1 = new Constant(new ConstantValue("1"));
  const_list->push_back(cv1);
  Variable* var_y = new Variable(new VariableName("y"));
  var_list->push_back(var_y);

  std::string stmt3_s = "1";
  std::vector<Variable*> stmt3_var_expr;
  std::vector<Constant*> stmt3_cv_expr;
  stmt3_cv_expr.push_back(cv1);

  AssignEntity* stmt3 = new AssignEntity(var_y, stmt3_s, stmt3_var_expr, stmt3_cv_expr);
  stmt3->SetStatementNumber(new StatementNumber(3));
  stmt3->SetBeforeNode(stmt2);
  proc->AddStatement(stmt3);
  deliverable->AddStatement(stmt3);
  deliverable->AddAssignEntity(stmt3);
  deliverable->AddModifiesRelationship(stmt3, stmt3->GetVariableObj());

  for (Variable* v: stmt3->GetExprVariables()) {
    deliverable->AddUsesRelationship(stmt3, v);
  }

  // z = 3
  Constant* cv2 = new Constant(new ConstantValue("3"));
  const_list->push_back(cv2);
  Variable* var_z = new Variable(new VariableName("z"));
  var_list->push_back(var_z);

  std::string stmt4_s = "3";
  std::vector<Variable*> stmt4_var_expr;
  std::vector<Constant*> stmt4_cv_expr;
  stmt3_cv_expr.push_back(cv2);

  AssignEntity* stmt4 = new AssignEntity(var_z, stmt4_s, stmt4_var_expr, stmt4_cv_expr);
  stmt4->SetStatementNumber(new StatementNumber(4));
  stmt4->SetBeforeNode(stmt3);
  proc->AddStatement(stmt4);
  deliverable->AddStatement(stmt4);
  deliverable->AddAssignEntity(stmt4);
  deliverable->AddModifiesRelationship(stmt4, stmt4->GetVariableObj());

  for (Variable* v: stmt4->GetExprVariables()) {
    deliverable->AddUsesRelationship(stmt4, v);
  }

  // z = x + y + 3;
  Constant* cv3 = new Constant(new ConstantValue("3")); //note that Constant is not unique.
  const_list->push_back(cv3);

  std::string stmt5_s = "x + y + 3";
  std::vector<Variable*> stmt5_var_expr;
  stmt5_var_expr.push_back(var_x);
  stmt5_var_expr.push_back(var_y);
  std::vector<Constant*> stmt5_cv_expr;
  stmt3_cv_expr.push_back(cv2);

  AssignEntity* stmt5 = new AssignEntity(var_z, stmt5_s, stmt5_var_expr, stmt5_cv_expr);
  stmt5->SetStatementNumber(new StatementNumber(5));
  stmt5->SetBeforeNode(stmt4);
  proc->AddStatement(stmt5);
  deliverable->AddStatement(stmt5);
  deliverable->AddAssignEntity(stmt5);
  deliverable->AddModifiesRelationship(stmt5, stmt5->GetVariableObj());

  for (Variable* v: stmt5->GetExprVariables()) {
    deliverable->AddUsesRelationship(stmt5, v);
  }

  deliverable->AddFollowRelationship(stmt1, stmt2);
  deliverable->AddFollowRelationship(stmt2, stmt3);
  deliverable->AddFollowRelationship(stmt3, stmt4);
  deliverable->AddFollowRelationship(stmt4, stmt5);

  return deliverable;
}

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
Deliverable* SetUpDeliverable_Transitive() {
  // Create Deliverable object
  Deliverable* deliverable = new Deliverable();

  // Create entities

  // procedure
  Procedure* Transitive = new Procedure(new ProcedureName("Transitive"));

  // variables
  Variable* var_y = new Variable(new VariableName("y"));
  Variable* var_q = new Variable(new VariableName("q"));
  Variable* var_z = new Variable(new VariableName("z"));
  Variable* var_x = new Variable(new VariableName("x"));
  Variable* var_p = new Variable(new VariableName("p"));

  // constants
  Constant* cv_2 = new Constant(new ConstantValue("2"));
  Constant* cv_1 = new Constant(new ConstantValue("1"));
  Constant* cv_24 = new Constant(new ConstantValue("24"));
  Constant* cv_0 = new Constant(new ConstantValue("0"));
  Constant* cv_5 = new Constant(new ConstantValue("5"));

  // assign
  // 1
  std::vector<Variable*> var_list_1 ({var_z});
  std::vector<Constant*> cv_list_1 ({cv_2});
  AssignEntity* stmt1 = new AssignEntity(var_y, "y=2*z", var_list_1, cv_list_1);

  // 3
  std::vector<Variable*> var_list_3 ({var_x});
  std::vector<Constant*> cv_list_3 ({cv_24});
  AssignEntity* stmt3 = new AssignEntity(var_z, "z=x*24", var_list_3, cv_list_3);

  // 5
  std::vector<Variable*> var_list_5 ({var_x, var_q});
  std::vector<Constant*> cv_list_5 ({cv_5});
  AssignEntity* stmt5 = new AssignEntity(var_y, "y=x*q-5", var_list_5, cv_list_5);

  // 6
  std::vector<Variable*> var_list_6 ({var_x, var_y, var_z, var_p, var_q});
  std::vector<Constant*> cv_list_6 ({});
  AssignEntity* stmt6 = new AssignEntity(var_y, "y=x+y*z+p*q", var_list_6, cv_list_6);

  // while
  // 4
  std::vector<Variable*> var_list_4 ({var_z});
  std::vector<Constant*> cv_list_4 ({cv_0});
  WhileEntity* stmt4 = new WhileEntity("z<0", var_list_4, cv_list_4);
  stmt4->AddStatement(stmt5);

  // if
  // else
  ElseEntity* stmt2_else = new ElseEntity();
  stmt2_else->AddStatement(stmt4);
  stmt2_else->AddStatement(stmt6);

  std::vector<Variable*> var_list_2 ({var_q});
  std::vector<Constant*> cv_list_2 ({cv_1});
  IfEntity* stmt2 = new IfEntity("q!=1", var_list_2, cv_list_2);
  stmt2->AddStatement(stmt3);
  stmt2->SetElseEntity(stmt2_else);

  // Add statements to Procedure

  Transitive->AddStatement(stmt1);
  Transitive->AddStatement(stmt2);

  Program* program = new Program(Transitive);
  deliverable->SetProgram(program);

  // Add entities to Deliverable

  // Add procedure

  deliverable->GetProcList()->push_back(Transitive);

  // Add variables

  deliverable->GetVariableList()->push_back(var_y);
  deliverable->GetVariableList()->push_back(var_q);
  deliverable->GetVariableList()->push_back(var_z);
  deliverable->GetVariableList()->push_back(var_x);
  deliverable->GetVariableList()->push_back(var_p);

  // Add constants

  deliverable->GetConstantList()->push_back(cv_2);
  deliverable->GetConstantList()->push_back(cv_1);
  deliverable->GetConstantList()->push_back(cv_24);
  deliverable->GetConstantList()->push_back(cv_0);
  deliverable->GetConstantList()->push_back(cv_5);

  // Add statement numbers to statements
  stmt1->SetStatementNumber(new StatementNumber(1));
  stmt2->SetStatementNumber(new StatementNumber(2));
  stmt3->SetStatementNumber(new StatementNumber(3));
  stmt4->SetStatementNumber(new StatementNumber(4));
  stmt5->SetStatementNumber(new StatementNumber(5));
  stmt6->SetStatementNumber(new StatementNumber(6));

  // Add statements

  deliverable->AddStatement(stmt1);
  deliverable->AddStatement(stmt2);
  deliverable->AddStatement(stmt3);
  deliverable->AddStatement(stmt4);
  deliverable->AddStatement(stmt5);
  deliverable->AddStatement(stmt6);

  // Add statement-type entities

  deliverable->AddAssignEntity(stmt1);
  deliverable->AddIfEntity(stmt2);
  deliverable->AddAssignEntity(stmt3);
  deliverable->AddWhileEntity(stmt4);
  deliverable->AddAssignEntity(stmt5);
  deliverable->AddAssignEntity(stmt6);

  // Add Follow relationships

  deliverable->AddFollowRelationship(stmt1, stmt2);
  deliverable->AddFollowRelationship(stmt4, stmt6);

  // Add Follow* relationships

  deliverable->AddFollowsTransitiveRelationship(stmt1, stmt2);
  deliverable->AddFollowsTransitiveRelationship(stmt4, stmt6);

  // Add Parent relationships

  deliverable->AddParentRelationship(stmt2, stmt3);
  deliverable->AddParentRelationship(stmt2, stmt4);
  deliverable->AddParentRelationship(stmt2, stmt6);
  deliverable->AddParentRelationship(stmt4, stmt5);

  // Add Parent* relationships

  deliverable->AddParentTransitiveRelationship(stmt2, stmt3);
  deliverable->AddParentTransitiveRelationship(stmt2, stmt4);
  deliverable->AddParentTransitiveRelationship(stmt2, stmt6);
  deliverable->AddParentTransitiveRelationship(stmt4, stmt5);
  deliverable->AddParentTransitiveRelationship(stmt2, stmt5);

  // Add Uses Relationship

  // For non-container statements
  deliverable->AddUsesRelationship(stmt1, var_z);
  deliverable->AddUsesRelationship(stmt3, var_x);
  deliverable->AddUsesRelationship(stmt5, var_x);
  deliverable->AddUsesRelationship(stmt5, var_q);
  deliverable->AddUsesRelationship(stmt6, var_x);
  deliverable->AddUsesRelationship(stmt6, var_y);
  deliverable->AddUsesRelationship(stmt6, var_z);
  deliverable->AddUsesRelationship(stmt6, var_p);
  deliverable->AddUsesRelationship(stmt6, var_q);

  // For container statements
  std::list<Variable*> usedBy2 ({var_x, var_y, var_z, var_p, var_q});
  deliverable->AddUsesRelationship(stmt2, &usedBy2);
  std::list<Variable*> usedBy4 ({var_x, var_q});
  deliverable->AddUsesRelationship(stmt4, &usedBy4);

  // Add Modifies Relationship

  // For non-container statements
  deliverable->AddModifiesRelationship(stmt1, var_y);
  deliverable->AddModifiesRelationship(stmt3, var_z);
  deliverable->AddModifiesRelationship(stmt5, var_y);
  deliverable->AddModifiesRelationship(stmt6, var_y);

  // For container statements
  std::list<Variable*> modifiedBy2 ({var_z, var_y});
  deliverable->AddModifiesRelationship(stmt2, &modifiedBy2);

  return deliverable;
}