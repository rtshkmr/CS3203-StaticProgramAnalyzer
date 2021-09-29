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
    ConstantValue* cv_2 = new ConstantValue("2");
    ConstantValue* cv_1 = new ConstantValue("1");
    ConstantValue* cv_24 = new ConstantValue("24");
    ConstantValue* cv_0 = new ConstantValue("0");
    ConstantValue* cv_5 = new ConstantValue("5");

    // assign
    // 1
    std::vector<Variable*> var_list_1 ({var_z});
    std::vector<ConstantValue*> cv_list_1 ({cv_2});
    AssignEntity* stmt1 = new AssignEntity(var_y, "y=2*z", var_list_1, cv_list_1);

    // 3
    std::vector<Variable*> var_list_3 ({var_x});
    std::vector<ConstantValue*> cv_list_3 ({cv_24});
    AssignEntity* stmt3 = new AssignEntity(var_z, "z=x*24", var_list_3, cv_list_3);

    // 5
    std::vector<Variable*> var_list_5 ({var_x, var_q});
    std::vector<ConstantValue*> cv_list_5 ({cv_5});
    AssignEntity* stmt5 = new AssignEntity(var_y, "y=x*q-5", var_list_5, cv_list_5);

    // 6
    std::vector<Variable*> var_list_6 ({var_x, var_y, var_z, var_p, var_q});
    std::vector<ConstantValue*> cv_list_6 ({});
    AssignEntity* stmt6 = new AssignEntity(var_y, "y=x+y*z+p*q", var_list_6, cv_list_6);

    // while
    // 4
    std::vector<Variable*> var_list_4 ({var_z});
    std::vector<ConstantValue*> cv_list_4 ({cv_0});
    WhileEntity* stmt4 = new WhileEntity("z<0", var_list_4, cv_list_4);
    stmt4->AddStatement(stmt5);

    // if
    // else
    ElseEntity* stmt2_else = new ElseEntity();
    stmt2_else->AddStatement(stmt4);
    stmt2_else->AddStatement(stmt6);

    std::vector<Variable*> var_list_2 ({var_q});
    std::vector<ConstantValue*> cv_list_2 ({cv_1});
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

    deliverable->proc_list_.push_back(Transitive);

    // Add variables

    deliverable->var_list_.push_back(var_y);
    deliverable->var_list_.push_back(var_q);
    deliverable->var_list_.push_back(var_z);
    deliverable->var_list_.push_back(var_x);
    deliverable->var_list_.push_back(var_p);

    // Add constants

    deliverable->const_list_.push_back(cv_2);
    deliverable->const_list_.push_back(cv_1);
    deliverable->const_list_.push_back(cv_24);
    deliverable->const_list_.push_back(cv_0);
    deliverable->const_list_.push_back(cv_5);

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

//TEST_CASE("3.QueryProcessor.Query Relationship Existence") {
//  Deliverable* deliverable = SetUpDeliverable_Transitive();
//  PKB pkb = PKB();
//  pkb.PopulateDataStructures(* deliverable);
//
//  SECTION("For Parent") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kParent);
//    REQUIRE(result);
//  }
//  SECTION("For ParentT") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kParentT);
//    REQUIRE(result);
//  }
//  SECTION("For Follows") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kFollows);
//    REQUIRE(result);
//  }
//  SECTION("For FollowsT") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kFollowsT);
//    REQUIRE(result);
//  }
//  SECTION("For UsesS") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kUsesS);
//    REQUIRE(result);
//  }
////  SECTION("For UsesP") {
////    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kUsesP);
////    REQUIRE(result);
////  }
//  SECTION("For ModifiesS") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kModifiesS);
//    REQUIRE(result);
//  }
////  SECTION("For ModifiesP") {
////    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kModifiesP);
////    REQUIRE(result);
////  }
//}
