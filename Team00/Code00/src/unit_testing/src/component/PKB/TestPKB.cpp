#include "catch.hpp"
#include <iostream>
#include <component/PKB/PKB.h>
#include <datatype/Deliverable.h>

TEST_CASE("PKB population and retrieval") {

    Deliverable d1_ = Deliverable();

    Procedure *p1_ = new Procedure(new ProcedureName("p1"));
    Variable *v1_ = new Variable(new VariableName("v1"));
    Variable *v2_ = new Variable(new VariableName("v2"));
    ConstantValue *const1_ = new ConstantValue("1");
    ConstantValue *const2_ = new ConstantValue("2");

    ReadEntity *read1_ = new ReadEntity(v1_);
    AssignEntity *a1_ = new AssignEntity
        (v1_,
         "v1=1+2+v2",
         std::vector<Variable *>{v2_},
         std::vector<ConstantValue *>{const1_, const2_}
        );
    CallEntity *call1_ = new CallEntity(p1_);
    PrintEntity *print1_ = new PrintEntity(v1_);

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


    SECTION("Get DesignEntity") {

        SECTION("Procedure") {
            std::list<std::string> proc_list{"p1"};
            REQUIRE(proc_list == pkb.GetDesignEntity(DesignEntity::kProcedure));
        }

        SECTION("Variable") {
            std::list<std::string> var_list{"v1", "v2"};
            REQUIRE(var_list == pkb.GetDesignEntity(DesignEntity::kVariable));
        }

        SECTION("Constant") {
            std::list<std::string> const_list{"1", "2"};
            REQUIRE(const_list == pkb.GetDesignEntity(DesignEntity::kConstant));
        }

        SECTION("Statement") {
            std::list<std::string> stmt_list{"1", "2", "3", "4"};
            REQUIRE(stmt_list == pkb.GetDesignEntity(DesignEntity::kStmt));
        }

        SECTION("Read") {
            std::list<std::string> read_list{"1"};
            REQUIRE(read_list == pkb.GetDesignEntity(DesignEntity::kRead));
        }

        SECTION("Assign") {
            std::list<std::string> assign_list{"2"};
            REQUIRE(assign_list == pkb.GetDesignEntity(DesignEntity::kAssign));
        }

        SECTION("Call") {
            std::list<std::string> call_list{"3"};
            REQUIRE(call_list == pkb.GetDesignEntity(DesignEntity::kCall));
        }

        SECTION("Print") {
            std::list<std::string> print_list{"4"};
            REQUIRE(print_list == pkb.GetDesignEntity(DesignEntity::kPrint));
        }

    }

    SECTION("Get Relationship") {
        SECTION("Follows") {
            std::list<std::tuple<DesignEntity, std::string>> follows_list{std::make_tuple(DesignEntity::kAssign, "2")};
            REQUIRE(follows_list == pkb.GetFollows("1"));
        }

        SECTION("FollowedBy") {
            std::list<std::tuple<DesignEntity, std::string>> previous_list{std::make_tuple(DesignEntity::kRead, "1")};
            REQUIRE(previous_list == pkb.GetPrevious("2"));
        }

        SECTION("Uses") {
            std::list<std::tuple<DesignEntity, std::string>> uses_list{std::make_tuple(DesignEntity::kVariable, "v1")};
            REQUIRE(uses_list == pkb.GetUses("4"));
        }

        SECTION("UsedBy") {
            std::list<std::tuple<DesignEntity, std::string>> uses_list{std::make_tuple(DesignEntity::kAssign, "2")};
            REQUIRE(uses_list == pkb.GetUsedBy("v2"));
        }

        SECTION("Modifies") {
            std::list<std::tuple<DesignEntity, std::string>> uses_list{std::make_tuple(DesignEntity::kVariable, "v1")};
            REQUIRE(uses_list == pkb.GetModifies("1"));
        }

        SECTION("ModifiedBy") {
            std::list<std::tuple<DesignEntity, std::string>> uses_list{};
            uses_list.push_back(std::make_tuple(DesignEntity::kRead, "1"));
            uses_list.push_back(std::make_tuple(DesignEntity::kAssign, "2"));
            REQUIRE(uses_list == pkb.GetModifiedBy("v1"));
        }
    }
}
