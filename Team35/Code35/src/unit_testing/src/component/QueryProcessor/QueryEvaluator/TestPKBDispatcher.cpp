#include "catch.hpp"
#include "component/QueryProcessor/QueryEvaluator/PKBDispatcher.h"
#include "component/PKB/PKB.h"
#include "util/PKBStub.h"

TEST_CASE("3.QueryProcessor.Query Relationship Existence") {
  Deliverable* deliverable = SetUpDeliverable_Transitive();
  PKB pkb = PKB();
  pkb.PopulateDataStructures(* deliverable);

  SECTION("For Parent") {
    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kParent);
    REQUIRE(result);
  }
  SECTION("For ParentT") {
    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kParentT);
    REQUIRE(result);
  }
  SECTION("For Follows") {
    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kFollows);
    REQUIRE(result);
  }
  SECTION("For FollowsT") {
    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kFollowsT);
    REQUIRE(result);
  }
  SECTION("For UsesS") {
    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kUsesS);
    REQUIRE(result);
  }
//  SECTION("For UsesP") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kUsesP);
//    REQUIRE(result);
//  }
  SECTION("For ModifiesS") {
    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kModifiesS);
    REQUIRE(result);
  }
//  SECTION("For ModifiesP") {
//    bool result = QueryPkbForRelationshipExistence(pkb, RelRef::kModifiesP);
//    REQUIRE(result);
//  }
}
