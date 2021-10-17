#include "catch.hpp"
#include "component/QueryProcessor/QueryExtractor/QueryExtractor.h"
#include "component/QueryProcessor/types/Types.h"

Group* Get(std::vector<Group*>* list, int index){
  auto it = list->begin();
  for(int i=0; i<index; i++){
    ++it;
  }
  return *it;
}

TEST_CASE("3.QueryGrouper.No and + multiple queries + common target synonym") {
  std::string query = "assign a; while w; Select a "
                      "such that Parent* (w, a) pattern a (\"count\", _) "
                      "pattern a (\"x\", _) such that Uses (a, \"x\")";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  // expecting 1 group with 4 clauses
  REQUIRE(groups.size() == 1);
  Group* g = Get(&groups, 0);
  REQUIRE(g->GetClauses().size() == 4);
  REQUIRE(g->ContainsTargetSynonym());
}

TEST_CASE("3.QueryGrouper.No and + multiple queries + common non-target synonym") {
  std::string query = "assign a, a2; while w; Select a2 "
                      "such that Parent* (w, a) pattern a (\"count\", _) "
                      "pattern a (\"x\", _) such that Uses (a, \"x\")";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  // expecting 2 groups, 1 is an empty group for tgt syn a2, the other is the boolean group with 4 clauses
  REQUIRE(groups.size() == 2);
  Group* g1 = Get(&groups, 0);
  Group* g2 = Get(&groups, 1);
  REQUIRE(g1->GetClauses().size() == 0);
  REQUIRE(g1->ContainsTargetSynonym());
  REQUIRE(g2->GetClauses().size() == 4);
  REQUIRE(g2->ContainsTargetSynonym() == false);
}

TEST_CASE("3.QueryGrouper.No and + multiple queries + common target & non-target synonym") {
  std::string query = "assign a1, a2; while w1, w2; Select a1 "
                      "such that Parent* (w1, a1) pattern a1 (\"count\", _) "
                      "pattern a2 (\"x\", _) such that Uses (a2, \"x\")";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  // expecting 2 groups with 2 clauses each
  REQUIRE(groups.size() == 2);
  Group* g1 = Get(&groups, 0);
  Group* g2 = Get(&groups, 1);
  REQUIRE(g1->GetClauses().size() == 2);
  REQUIRE(g1->ContainsTargetSynonym());
  REQUIRE(g2->GetClauses().size() == 2);
  REQUIRE(g2->ContainsTargetSynonym() == false);
}

TEST_CASE("3.QueryGrouper.And keyword + multiple queries rel & pattern") {
  std::string query = "assign a1, a2; while w1, w2; Select a2 "
                      "pattern a1 (\"x\", _) and a2 (\"x\",_\"x\"_) "
                      "such that Parent* (w2, a2) and Parent* (w1, w2)";
  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  // expecting 2 groups with 1 and 3 clauses respectively.
  REQUIRE(groups.size() == 2);
  Group* g1 = Get(&groups, 0);
  Group* g2 = Get(&groups, 1);
  REQUIRE(g1->GetClauses().size() == 3);
  REQUIRE(g1->ContainsTargetSynonym());
  REQUIRE(g2->GetClauses().size() == 1);
  REQUIRE(g2->ContainsTargetSynonym() == false);
}
