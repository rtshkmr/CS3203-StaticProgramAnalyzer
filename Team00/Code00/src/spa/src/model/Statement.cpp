/**
 * This file contains the implementation of commonly used statement types in SIMPLE program.
 */

#include "Statement.h"

If::If(std::string condition) {
    condExpr = new ConditionalExpression(condition);
}

ConditionalExpression *If::getCondExpr() {
    return condExpr;
}

std::vector<Statement> *If::getIfStmtList() {
    return &ifStmtList;
}

std::vector<Statement> *If::getElseStmtList() {
    return elseStmtList->getElseStmtList();
}

bool If::setElseStmtList(Else *elseStmt) {
    elseStmtList = elseStmt;
    return true; //assume success setter
}

Else::Else() {

}

std::vector<Statement> *Else::getElseStmtList() {
    return &elseStmtList;
}

While::While(std::string condition) {
    condExpr = new ConditionalExpression(condition);
}

ConditionalExpression *While::getCondExpr() {
    return condExpr;
}

std::vector<Statement> *While::getStmtList() {
    return &stmtList;
}

Assign::Assign(Variable *v, std::string expression) {
    assignTo = v;
    expr = new AssignmentExpression(expression);
}

Variable *Assign::getVariable() {
    return assignTo;
}

AssignmentExpression *Assign::getAssignmentExpr() {
    return expr;
}

Call::Call(Procedure *procName) {
    proc = procName;
}

Procedure *Call::getProcedure() {
    return proc;
}

Print::Print(Variable *vName) {
    var = vName;
}

Variable *Print::getVariable() {
    return var;
}

Read::Read(Variable *vName) {
    var = vName;
}

Variable *Read::getVariable() {
    return var;
}
