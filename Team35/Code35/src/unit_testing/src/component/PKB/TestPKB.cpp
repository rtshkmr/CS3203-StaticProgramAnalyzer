#include "catch.hpp"
#include <iostream>
#include <component/PKB/PKB.h>
#include <datatype/Deliverable.h>

TEST_CASE("2.PKB.PKB population and retrieval") {

  Deliverable d1_ = Deliverable();

  Procedure* p1_ = new Procedure(new ProcedureName("p1"));
  Variable* v1_ = new Variable(new VariableName("v1"));
  Variable* v2_ = new Variable(new VariableName("v2"));
  Constant* const1_ = new Constant(new ConstantValue("1"));
  Constant* const2_ = new Constant(new ConstantValue("2"));

  ReadEntity* read1_ = new ReadEntity(v1_);
  AssignEntity* a1_ = new AssignEntity
      (v1_,
       "v1=1+2+v2",
       std::vector<Variable*>{v2_},
       std::vector<Constant*>{const1_, const2_}
      );
  CallEntity* call1_ = new CallEntity(p1_);
  PrintEntity* print1_ = new PrintEntity(v1_);

  read1_->SetStatementNumber(new StatementNumber(1));
  a1_->SetStatementNumber(new StatementNumber(2));
  call1_->SetStatementNumber(new StatementNumber(3));
  print1_->SetStatementNumber(new StatementNumber(4));

  d1_.proc_list_.push_back(p1_);
  d1_.var_list_.push_back(v1_);
  d1_.var_list_.push_back(v2_);
  d1_.const_list_.push_back(const1_);
  d1_.const_list_.push_back(const2_);

  d1_.AddStatement(read1_);
  d1_.AddStatement(a1_);
  d1_.AddStatement(call1_);
  d1_.AddStatement(print1_);

  d1_.AddReadEntity(read1_);
  d1_.AddAssignEntity(a1_);
  d1_.AddCallEntity(call1_);
  d1_.AddPrintEntity(print1_);

  d1_.AddFollowRelationship(read1_, a1_);
  d1_.AddFollowRelationship(a1_, call1_);
  d1_.AddFollowRelationship(call1_, print1_);

  d1_.AddUsesRelationship(print1_, v1_);
  d1_.AddUsesRelationship(a1_, v2_);

  d1_.AddModifiesRelationship(read1_, v1_);
  d1_.AddModifiesRelationship(a1_, v1_);

  PKB pkb = PKB();

  pkb.PopulateDataStructures(d1_);

  // Check if PKB can store and retrieve individual elements (e.g. read, print, constant, variable)
  SECTION("Get DesignEntity") {

    SECTION("Procedure") {
      std::vector<Entity*> proc_list;
      proc_list.push_back(p1_);
      REQUIRE(proc_list == pkb.GetDesignEntities(DesignEntity::kProcedure));
    }

    SECTION("Variable") {
      std::vector<Entity*> var_list;
      var_list.push_back(v1_);
      var_list.push_back(v2_);
      REQUIRE(var_list == pkb.GetDesignEntities(DesignEntity::kVariable));
    }

    SECTION("Constant") {
      std::vector<Entity*> pkb_const = pkb.GetDesignEntities(DesignEntity::kConstant);
      REQUIRE(pkb_const.size() == 2);
      Constant* const1 = (Constant*) pkb_const.at(0);
      REQUIRE(const1 == const1_);
      Constant* const2 = (Constant*) pkb_const.at(1);
      REQUIRE(const2 == const2_);
    }

    SECTION("Statement") {
      std::vector<Entity*> stmt_list;
      stmt_list.push_back(read1_);
      stmt_list.push_back(a1_);
      stmt_list.push_back(call1_);
      stmt_list.push_back(print1_);
      REQUIRE(stmt_list == pkb.GetDesignEntities(DesignEntity::kStmt));
    }

    SECTION("Read") {
      std::vector<Entity*> read_list;
      read_list.push_back(read1_);
      REQUIRE(read_list == pkb.GetDesignEntities(DesignEntity::kRead));
    }

    SECTION("Assign") {
      std::vector<Entity*> assign_list;
      assign_list.push_back(a1_);
      REQUIRE(assign_list == pkb.GetDesignEntities(DesignEntity::kAssign));
    }

    SECTION("Call") {
      std::vector<Entity*> call_list;
      call_list.push_back(call1_);
      REQUIRE(call_list == pkb.GetDesignEntities(DesignEntity::kCall));
    }

    SECTION("Print") {
      std::vector<Entity*> print_list;
      print_list.push_back(print1_);
      REQUIRE(print_list == pkb.GetDesignEntities(DesignEntity::kPrint));
    }

  }

  // Check if PKB can store and retrieve relationships
  SECTION("Get Relationship") {
    SECTION("Follows") {
      std::vector<Entity*> follows_list;
      follows_list.push_back(a1_);
      REQUIRE(follows_list == pkb.GetRelationship(PKBRelRefs::kFollows, "1"));
    }

    SECTION("FollowedBy") {
        std::vector<Entity*> followed_by_list;
        followed_by_list.push_back(read1_);
        REQUIRE(followed_by_list == pkb.GetRelationship(PKBRelRefs::kFollowedBy, "2"));
    }
  }

  // Check if PKB can store and retrieve the existence of any relationship
  SECTION("Has Relationship") {
    SECTION("Follows") {
      REQUIRE(pkb.HasRelationship(PKBRelRefs::kFollows));
    }

    SECTION("Previous") {
      REQUIRE(pkb.HasRelationship(PKBRelRefs::kFollowedBy));
    }

    SECTION("Parent") {
      REQUIRE(!pkb.HasRelationship(PKBRelRefs::kParent));
    }

    SECTION("Child") {
      REQUIRE(!pkb.HasRelationship(PKBRelRefs::kChild));
    }
  }

  // Check if PKB can store and retrieve assign expressions given either variable name or statement number
  SECTION("Assignment Expressions") {
    SECTION("Get pattern by assignment statement number") {
      std::vector<Entity*> assign_vector = pkb.GetPatternEntities(DesignEntity::kAssign, "2");
      REQUIRE(assign_vector.size() == 1);
      REQUIRE(assign_vector[0] == a1_);
    }

    SECTION("Get pattern by variable name") {
      std::vector<Entity*> assign_vector = pkb.GetPatternEntities(DesignEntity::kAssign, "v1");
      REQUIRE(assign_vector.size() == 1);
      REQUIRE(assign_vector[0] == a1_);
    }
  }
}
