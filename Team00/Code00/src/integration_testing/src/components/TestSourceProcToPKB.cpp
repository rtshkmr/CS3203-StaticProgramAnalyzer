#include "catch.hpp"
#include "component/SourceProcessor/SourceProcessor.h"
#include "component/SourceProcessor/Parser.h"
#include "component/SourceProcessor/DesignExtractor.h"
#include "component/PKB/PKB.h"
#include <fstream>

TEST_CASE("Source processing to PKB retrieval") {

    std::ifstream source_file ("../../../tests/basic_source.txt");

    REQUIRE(source_file.good());

    par::Parser parser;
    parser.Parse("../../../tests/basic_source.txt");
    Deliverable deliverable = parser.GetDeliverables();
    de::DesignExtractor design_extractor;
    design_extractor.ExtractDesignAbstractions();
    PKB pkb = PKB();
    pkb.PopulateDataStructures(deliverable);
    SECTION("Procedure") {
        REQUIRE(std::list<std::string>{"Week4"} == pkb.GetDesignEntity(DesignEntity::kProcedure));
    }

    SECTION("Variable") {
        REQUIRE(std::list<std::string>{"x", "y", "z"} == pkb.GetDesignEntity(DesignEntity::kVariable));
    }

    SECTION("Constant") {
        REQUIRE(std::list<std::string>{"1", "3", "3"} == pkb.GetDesignEntity(DesignEntity::kConstant));
    }

    SECTION("Statement") {
        REQUIRE(std::list<std::string>{"1", "2", "3", "4", "5"} == pkb.GetDesignEntity(DesignEntity::kStmt));
    }

    SECTION("If") {
        REQUIRE(std::list<std::string>{} == pkb.GetDesignEntity(DesignEntity::kIf));
    }

    SECTION("While") {
        REQUIRE(std::list<std::string>{} == pkb.GetDesignEntity(DesignEntity::kWhile));
    }

    SECTION("Assign") {
        REQUIRE(std::list<std::string>{"3", "4", "5"} == pkb.GetDesignEntity(DesignEntity::kAssign));
    }

    SECTION("Call") {
        REQUIRE(std::list<std::string>{} == pkb.GetDesignEntity(DesignEntity::kCall));
    }

    SECTION("Print") {
        REQUIRE(std::list<std::string>{"2"} == pkb.GetDesignEntity(DesignEntity::kPrint));
    }

    SECTION("Read") {
        REQUIRE(std::list<std::string>{"1"} == pkb.GetDesignEntity(DesignEntity::kRead));
    }

    SECTION("Follows") {
        REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kPrint, "2")} == pkb.GetFollows("1"));
        REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kAssign, "3")} == pkb.GetFollows("2"));
        REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kAssign, "4")} == pkb.GetFollows("3"));
        REQUIRE(std::list<std::tuple<DesignEntity, std::string>>{std::make_tuple(DesignEntity::kAssign, "5")} == pkb.GetFollows("4"));
    }
}