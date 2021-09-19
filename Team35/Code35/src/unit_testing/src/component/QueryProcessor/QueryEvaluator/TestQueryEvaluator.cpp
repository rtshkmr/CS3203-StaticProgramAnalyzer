//
// Created by Max Ng on 11/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/QueryEvaluator/QueryEvaluator.h"
#include "component/QueryProcessor/types/Types.h"
#include "component/PKB/PKB.h"
#include <datatype/Deliverable.h>

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
    std::list<ConstantValue*>* const_list = deliverable->GetConstantValueList();

    // read x
    Variable* var_x = new Variable(new VariableName("x"));
    var_list->push_back(var_x);

    ReadEntity* stmt1 = new ReadEntity(var_x);
    stmt1->SetStatementNumber(new StatementNumber(1));
    proc->AddStatement(stmt1);
    deliverable->AddStatement(stmt1);
    deliverable->AddReadEntity(stmt1);
    deliverable->AddModifiesRelationship(stmt1, stmt1->GetVariable());

    // print x
    PrintEntity* stmt2 = new PrintEntity(var_x);
    stmt2->SetStatementNumber(new StatementNumber(2));
    stmt2->SetBeforeNode(stmt1);
    proc->AddStatement(stmt2);
    deliverable->AddStatement(stmt2);
    deliverable->AddPrintEntity(stmt2);
    deliverable->AddUsesRelationship(stmt2, stmt2->GetVariable());

    // y = 1
    ConstantValue* cv1 = new ConstantValue("1");
    const_list->push_back(cv1);
    Variable* var_y = new Variable(new VariableName("y"));
    var_list->push_back(var_y);

    std::string stmt3_s = "1";
    std::vector<Variable*> stmt3_var_expr;
    std::vector<ConstantValue*> stmt3_cv_expr;
    stmt3_cv_expr.push_back(cv1);

    AssignEntity* stmt3 = new AssignEntity(var_y, stmt3_s, stmt3_var_expr, stmt3_cv_expr);
    stmt3->SetStatementNumber(new StatementNumber(3));
    stmt3->SetBeforeNode(stmt2);
    proc->AddStatement(stmt3);
    deliverable->AddStatement(stmt3);
    deliverable->AddAssignEntity(stmt3);
    deliverable->AddModifiesRelationship(stmt3, stmt3->GetVariable());

    for (Variable* v: stmt3->GetExpressionVariables()) {
        deliverable->AddUsesRelationship(stmt3, v);
    }

    // z = 3
    ConstantValue* cv2 = new ConstantValue("3");
    const_list->push_back(cv2);
    Variable* var_z = new Variable(new VariableName("z"));
    var_list->push_back(var_z);

    std::string stmt4_s = "3";
    std::vector<Variable*> stmt4_var_expr;
    std::vector<ConstantValue*> stmt4_cv_expr;
    stmt3_cv_expr.push_back(cv2);

    AssignEntity* stmt4 = new AssignEntity(var_z, stmt4_s, stmt4_var_expr, stmt4_cv_expr);
    stmt4->SetStatementNumber(new StatementNumber(4));
    stmt4->SetBeforeNode(stmt3);
    proc->AddStatement(stmt4);
    deliverable->AddStatement(stmt4);
    deliverable->AddAssignEntity(stmt4);
    deliverable->AddModifiesRelationship(stmt4, stmt4->GetVariable());

    for (Variable* v: stmt4->GetExpressionVariables()) {
        deliverable->AddUsesRelationship(stmt4, v);
    }

    // z = x + y + 3;
    ConstantValue* cv3 = new ConstantValue("3"); //note that ConstantValue is not unique.
    const_list->push_back(cv3);

    std::string stmt5_s = "x + y + 3";
    std::vector<Variable*> stmt5_var_expr;
    stmt5_var_expr.push_back(var_x);
    stmt5_var_expr.push_back(var_y);
    std::vector<ConstantValue*> stmt5_cv_expr;
    stmt3_cv_expr.push_back(cv2);

    AssignEntity* stmt5 = new AssignEntity(var_z, stmt5_s, stmt5_var_expr, stmt5_cv_expr);
    stmt5->SetStatementNumber(new StatementNumber(5));
    stmt5->SetBeforeNode(stmt4);
    proc->AddStatement(stmt5);
    deliverable->AddStatement(stmt5);
    deliverable->AddAssignEntity(stmt5);
    deliverable->AddModifiesRelationship(stmt5, stmt5->GetVariable());

    for (Variable* v: stmt5->GetExpressionVariables()) {
        deliverable->AddUsesRelationship(stmt5, v);
    }

    deliverable->AddFollowRelationship(stmt1, stmt2);
    deliverable->AddFollowRelationship(stmt2, stmt3);
    deliverable->AddFollowRelationship(stmt3, stmt4);
    deliverable->AddFollowRelationship(stmt4, stmt5);

    return deliverable;
}


TEST_CASE("3.QueryProcessor.Query select only") {
    Deliverable d1_ = Deliverable();

    Procedure *p1_ = new Procedure(new ProcedureName("p1"));
    Variable *v1_ = new Variable(new VariableName("v1"));
    Variable *v2_ = new Variable(new VariableName("v2"));
    ConstantValue *const1_ = new ConstantValue("1");
    ConstantValue *const2_ = new ConstantValue("2");

    AssignEntity *a1_ = new AssignEntity
        (v1_,
         "v1=1+2",
         std::vector<Variable *>{},
         std::vector<ConstantValue *>{const1_, const2_}
         );
    CallEntity *call1_ = new CallEntity(p1_);
    PrintEntity *print1_ = new PrintEntity(v1_);
    ReadEntity *read1_ = new ReadEntity(v1_);

    a1_->SetStatementNumber(new StatementNumber(1));
    call1_->SetStatementNumber(new StatementNumber(2));
    print1_->SetStatementNumber(new StatementNumber(3));
    read1_->SetStatementNumber(new StatementNumber(4));

    //  d1_.AddProc(p1_);
    //  d1_.AddVariable(v1_);
    //  d1_.AddVariable(v2_);
    //  d1_.AddConstant(const1_);
    //  d1_.AddConstant(const2_);

    d1_.var_list_.push_back(v1_);
    d1_.var_list_.push_back(v2_);

    d1_.AddStatement(a1_);
    d1_.AddStatement(call1_);
    d1_.AddStatement(print1_);
    d1_.AddStatement(read1_);

    d1_.AddAssignEntity(a1_);
    d1_.AddCallEntity(call1_);
    d1_.AddPrintEntity(print1_);
    d1_.AddReadEntity(read1_);

    d1_.AddFollowRelationship(a1_, call1_);
    d1_.AddFollowRelationship(call1_, print1_);
    d1_.AddFollowRelationship(print1_, read1_);

    PKB pkb = PKB();

    pkb.PopulateDataStructures(d1_);

    SECTION( "Basic select query" ) {
        std::list<Synonym> synonymList = {Synonym("s2", DesignEntity::kStmt),
                                          Synonym("a1", DesignEntity::kAssign),
                                          Synonym("call", DesignEntity::kCall)};
        std::list<Group*> groupList;

        SECTION( "Basic statement select query" ) {
            Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"1", "2", "3", "4"};
            REQUIRE(output == answer);
        }

        SECTION( "Basic assign statement select query" ) {

            Synonym targetSynonym = Synonym("a1", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"1"};
            REQUIRE(output == answer);
        }

        SECTION( "Basic call statement select query" ) {

            Synonym targetSynonym = Synonym("call", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"2"};
            REQUIRE(output == answer);
        }

    }

    SECTION( "Single SuchThat query" ) {
        std::list<Synonym> synonymList = {Synonym("s2", DesignEntity::kStmt),
                                          Synonym("a1", DesignEntity::kAssign),
                                          Synonym("call", DesignEntity::kCall)};
        struct SuchThat st;
        st.rel_ref = RelRef::kFollows;

        SECTION( "With double synonyms" ) {
            st.right_is_synonym = true;
            st.left_is_synonym = true;
            st.right_hand_side = "call";
            st.left_hand_side = "s2";
            std::vector<Clause*> clauseList = {&st};
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};

            Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"1"};
            REQUIRE(output == answer);
        }

        SECTION( "With single Synonym" ) {
            st.right_is_synonym = false;
            st.left_is_synonym = true;
            st.right_hand_side = "3";
            st.left_hand_side = "s2";

            std::vector<Clause*> clauseList = {&st};
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};

            Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"2"};
            REQUIRE(output == answer);
        }

        SECTION( "With no synonyms and both references with correct result" ) {
            st.right_is_synonym = false;
            st.left_is_synonym = false;
            st.right_hand_side = "3";
            st.left_hand_side = "2";

            std::vector<Clause*> clauseList = {&st};
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};

            Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"1", "2", "3", "4"};
            REQUIRE(output == answer);
        }

        SECTION( "With no synonyms and both references but wrong" ) {
            st.right_is_synonym = false;
            st.left_is_synonym = false;
            st.right_hand_side = "1";
            st.left_hand_side = "2";

            std::vector<Clause*> clauseList = {&st};
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};

            Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {};
            REQUIRE(output == answer);
        }

        SECTION( "With no synonyms and left side is _ with correct result") {
            st.right_is_synonym = false;
            st.left_is_synonym = false;
            st.right_hand_side = "3";
            st.left_hand_side = "_";

            std::vector<Clause*> clauseList = {&st};
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};

            Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"1", "2", "3", "4"};
            REQUIRE(output == answer);
        }

        SECTION( "With no synonyms and left side is _ with incorrect result") {
            st.right_is_synonym = false;
            st.left_is_synonym = false;
            st.right_hand_side = "1";
            st.left_hand_side = "_";

            std::vector<Clause*> clauseList = {&st};
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};

            Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {};
            REQUIRE(output == answer);
        }
    }
}

TEST_CASE( "3.QueryEvaluator.Evaluate single Follow and Parent(?) clauses" ) {
    Deliverable* deliverable = SetUpDeliverable_Week4();
    PKB pkb = PKB();
    pkb.PopulateDataStructures(*deliverable);
    std::list<Synonym> synonymList = {Synonym("s1", DesignEntity::kStmt),
                                      Synonym("a1", DesignEntity::kAssign),
                                      Synonym("a2", DesignEntity::kAssign),
                                      Synonym("p1", DesignEntity::kPrint),
                                      Synonym("v1", DesignEntity::kVariable),
                                      Synonym("r1", DesignEntity::kRead)};

    struct SuchThat st;
    st.rel_ref = RelRef::kFollows;

    SECTION( "Both Synonyms with correct results" ) {
        st.left_is_synonym = true;
        st.left_hand_side = "p1";

        st.right_is_synonym = true;
        st.right_hand_side = "a1";

        std::vector<Clause*> clauseList = {&st};

        SECTION("Select first synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("p1", DesignEntity::kPrint);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"2"};
            REQUIRE(output == answer);
        }

        SECTION("Select second synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"3"};
            REQUIRE(output == answer);
        }

        SECTION("Select neither synonym, with correct boolean result") {
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a2", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"3", "4", "5"};
            REQUIRE(output == answer);
        }
    }

    SECTION( "Both Synonyms with incorrect results" ) {
        st.left_is_synonym = true;
        st.left_hand_side = "p1";

        st.right_is_synonym = true;
        st.right_hand_side = "r1";

        std::vector<Clause*> clauseList = {&st};

        SECTION("Select first synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("p1", DesignEntity::kPrint);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {};
            REQUIRE(output == answer);
        }

        SECTION("Select second synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("r1", DesignEntity::kRead);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {};
            REQUIRE(output == answer);
        }

        SECTION("Select neither synonym, with incorrect boolean result") {
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("2", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {};
            REQUIRE(output == answer);
        }
    }

    SECTION( "First param is synonym, second is value with correct results" ) {
        st.left_is_synonym = true;
        st.left_hand_side = "a1";

        st.right_is_synonym = false;
        st.right_hand_side = "4";

        std::vector<Clause*> clauseList = {&st};

        SECTION("Select first synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"3"};
            REQUIRE(output == answer);
        }

        SECTION("Select synonym not in clause") {
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("r1", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"1"};
            REQUIRE(output == answer);
        }
    }

    SECTION( "First param is synonym, second is _ with correct results" ) {
        st.left_is_synonym = true;
        st.left_hand_side = "a1";

        st.right_is_synonym = false;
        st.right_hand_side = "_";

        std::vector<Clause*> clauseList = {&st};

        SECTION("Select first synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"3", "4"};
            REQUIRE(output == answer);
        }

        SECTION("Select synonym not in clause") {
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("r1", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"1"};
            REQUIRE(output == answer);
        }
    }

    SECTION( "Second param is synonym, first is value with correct results" ) {
        st.left_is_synonym = false;
        st.left_hand_side = "4";

        st.right_is_synonym = true;
        st.right_hand_side = "a1";

        std::vector<Clause*> clauseList = {&st};

        SECTION("Select second synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"5"};
            REQUIRE(output == answer);
        }

        SECTION("Select synonym not in clause") {
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a2", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"3", "4", "5"};
            REQUIRE(output == answer);
        }
    }

    SECTION( "Second param is synonym, first is _ with correct results" ) {
        st.left_is_synonym = false;
        st.left_hand_side = "_";

        st.right_is_synonym = true;
        st.right_hand_side = "a1";

        std::vector<Clause*> clauseList = {&st};

        SECTION("Select second synonym") {
            Group firstGroup (clauseList, true);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"3", "4", "5"};
            REQUIRE(output == answer);
        }

        SECTION("Select synonym not in clause") {
            Group firstGroup (clauseList, false);
            std::list<Group*> groupList = {&firstGroup};
            Synonym targetSynonym = Synonym("a2", DesignEntity::kAssign);
            QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

            std::vector<std::string> output = evaluator.EvaluateQuery();
            std::vector<std::string> answer = {"3", "4", "5"};
            REQUIRE(output == answer);
        }
    }

    SECTION( "None is synonym, both values, correct answer" ) {
        st.left_is_synonym = false;
        st.left_hand_side = "1";

        st.right_is_synonym = false;
        st.right_hand_side = "2";

        std::vector<Clause*> clauseList = {&st};

        Group firstGroup (clauseList, false);
        std::list<Group*> groupList = {&firstGroup};
        Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
        QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

        std::vector<std::string> output = evaluator.EvaluateQuery();
        std::vector<std::string> answer = {"3", "4", "5"};
        REQUIRE(output == answer);
    }

    SECTION( "None is synonym, both values, incorrect answer" ) {
        st.left_is_synonym = false;
        st.left_hand_side = "1";

        st.right_is_synonym = false;
        st.right_hand_side = "3";

        std::vector<Clause*> clauseList = {&st};

        Group firstGroup (clauseList, false);
        std::list<Group*> groupList = {&firstGroup};
        Synonym targetSynonym = Synonym("p1", DesignEntity::kPrint);
        QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

        std::vector<std::string> output = evaluator.EvaluateQuery();
        std::vector<std::string> answer = {};
        REQUIRE(output == answer);
    }

    SECTION( "None is synonym, single wildcard, incorrect answer" ) {
        st.left_is_synonym = false;
        st.left_hand_side = "_";

        st.right_is_synonym = false;
        st.right_hand_side = "1";

        std::vector<Clause*> clauseList = {&st};

        Group firstGroup (clauseList, false);
        std::list<Group*> groupList = {&firstGroup};
        Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
        QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

        std::vector<std::string> output = evaluator.EvaluateQuery();
        std::vector<std::string> answer = {};
        REQUIRE(output == answer);
    }

    SECTION( "None is synonym, single wildcard, incorrect answer" ) {
        st.left_is_synonym = false;
        st.left_hand_side = "_";

        st.right_is_synonym = false;
        st.right_hand_side = "1";

        std::vector<Clause*> clauseList = {&st};

        Group firstGroup (clauseList, false);
        std::list<Group*> groupList = {&firstGroup};
        Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
        QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

        std::vector<std::string> output = evaluator.EvaluateQuery();
        std::vector<std::string> answer = {};
        REQUIRE(output == answer);
    }

    SECTION( "None is synonym, doublle wildcard, correct answer" ) {
        st.left_is_synonym = false;
        st.left_hand_side = "_";

        st.right_is_synonym = false;
        st.right_hand_side = "_";

        std::vector<Clause*> clauseList = {&st};

        Group firstGroup (clauseList, false);
        std::list<Group*> groupList = {&firstGroup};
        Synonym targetSynonym = Synonym("a1", DesignEntity::kAssign);
        QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

        std::vector<std::string> output = evaluator.EvaluateQuery();
        std::vector<std::string> answer = {"3", "4", "5"};
        REQUIRE(output == answer);
    }
}

/* procedure Week4 {
 * read x;
 * print x;
 * y = 1;
 * z = 3;
 * z = x + y + 3;
 * }
 */
TEST_CASE( "3.QueryEvaluator.Evaluate single Uses and Modifies clauses" ) {
  Deliverable* deliverable = SetUpDeliverable_Week4();
  PKB pkb = PKB();
  pkb.PopulateDataStructures(*deliverable);
  std::list<Synonym> synonymList = {Synonym("s1", DesignEntity::kStmt),
                                    Synonym("a1", DesignEntity::kAssign),
                                    Synonym("v2", DesignEntity::kVariable),
                                    Synonym("p1", DesignEntity::kPrint),
                                    Synonym("v1", DesignEntity::kVariable),
                                    Synonym("r1", DesignEntity::kRead)};

  SECTION( "Both Synonyms with correct results" ) {
    struct SuchThat st;
    st.rel_ref = RelRef::kModifiesS;

    st.left_is_synonym = true;
    st.left_hand_side = "r1";

    st.right_is_synonym = true;
    st.right_hand_side = "v1";

    std::vector<Clause*> clauseList = {&st};

    SECTION("Select first synonym") {
      Group firstGroup (clauseList, true);
      std::list<Group*> group_list = {&firstGroup};
      Synonym target_synonym = Synonym("r1", DesignEntity::kRead);
      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);

      std::vector<std::string> output = evaluator.EvaluateQuery();
      std::vector<std::string> answer = {"1"};
      REQUIRE(output == answer);
    }

    SECTION("Select second synonym") {
      Group firstGroup (clauseList, true);
      std::list<Group*> group_list = {&firstGroup};
      Synonym target_synonym = Synonym("v1", DesignEntity::kVariable);
      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);

      std::vector<std::string> output = evaluator.EvaluateQuery();
      std::vector<std::string> answer = {"x"};
      REQUIRE(output == answer);
    }

    SECTION("Select neither synonym, with correct boolean result") {
      Group firstGroup (clauseList, false);
      std::list<Group*> group_list = {&firstGroup};
      Synonym target_synonym = Synonym("v2", DesignEntity::kAssign);
      QueryEvaluator evaluator(synonymList, target_synonym, group_list, pkb);

      std::vector<std::string> output = evaluator.EvaluateQuery();
      std::vector<std::string> answer = {"x", "y", "z"};
      REQUIRE(output == answer);
    }

  }
}
