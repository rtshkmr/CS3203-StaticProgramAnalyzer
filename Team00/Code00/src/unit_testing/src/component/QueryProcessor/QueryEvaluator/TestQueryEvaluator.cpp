//
// Created by Max Ng on 11/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/QueryEvaluator/QueryEvaluator.h"
#include "component/QueryProcessor/types/Types.h"
#include "component/PKB/PKB.h"
#include <datatype/Deliverable.h>


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
    std::list<Group> groupList;

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
      std::list<Group> groupList = {firstGroup};

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
      std::list<Group> groupList = {firstGroup};

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
      std::list<Group> groupList = {firstGroup};

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
      std::list<Group> groupList = {firstGroup};

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
      std::list<Group> groupList = {firstGroup};

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
      std::list<Group> groupList = {firstGroup};

      Synonym targetSynonym = Synonym("s2", DesignEntity::kStmt);
      QueryEvaluator evaluator(synonymList, targetSynonym, groupList, pkb);

      std::vector<std::string> output = evaluator.EvaluateQuery();
      std::vector<std::string> answer = {};
      REQUIRE(output == answer);
    }
  }
}