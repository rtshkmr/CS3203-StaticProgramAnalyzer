#include "catch.hpp"
#include <iostream>
#include <component/PKB/PKB.h>
#include <datatype/Deliverable.h>

using namespace std;


TEST_CASE("PKB population and retrieval") {

  Deliverable d1_ = Deliverable();

  Procedure *p1_ = new Procedure(new ProcedureName("p1"));
  Variable *v1_ = new Variable(new VariableName("v1"));
  Variable *v2_ = new Variable(new VariableName("v2"));
  ConstantValue *const1_ = new ConstantValue("1");
  ConstantValue *const2_ = new ConstantValue("2");

  AssignEntity *a1_ = new AssignEntity
      (v1_,
       "v1=1+2",
       vector<Variable*>{},
       vector<ConstantValue*>{const1_, const2_}
       );
  CallEntity *call1_ = new CallEntity(p1_);
  PrintEntity *print1_ = new PrintEntity(v1_);
  ReadEntity *read1_ = new ReadEntity(v1_);

  a1_->SetStatementNumber(new StatementNumber(1));
  call1_->SetStatementNumber(new StatementNumber(2));
  print1_->SetStatementNumber(new StatementNumber(3));
  read1_->SetStatementNumber(new StatementNumber(4));

  d1_.AddProc(p1_);
  d1_.AddVariable(v1_);
  d1_.AddVariable(v2_);
  d1_.AddConstant(const1_);
  d1_.AddConstant(const2_);

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


  SECTION("Get DesignEntity") {
    SECTION("Procedure") {
      list<string> proc_list {"p1"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kProcedure));
    }
    SECTION("Variable") {
      list<string> proc_list {"v1", "v2"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kVariable));
    }
    SECTION("Constant") {
      list<string> proc_list {"1", "2"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kConstant));
    }
    SECTION("Statement") {
      list<string> proc_list {"1", "2", "3", "4"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kStmt));
    }
    SECTION("Assign") {
      list<string> proc_list {"1"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kAssign));
    }
    SECTION("Call") {
      list<string> proc_list {"2"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kCall));
    }
    SECTION("Print") {
      list<string> proc_list {"3"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kPrint));
    }
    SECTION("Read") {
      list<string> proc_list {"4"};
      REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kRead));
    }
  }
  SECTION("Get Relationship") {
    SECTION("Follows") {
      list<tuple<DesignEntity,string>> follows_list {make_tuple(DesignEntity::kCall,"2")};
      REQUIRE(follows_list == pkb.GetFollows("1"));
    }
  }
}
