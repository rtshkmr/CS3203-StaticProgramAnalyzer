//
// Created by Hemanshu Gandhi on 8/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/QueryExtractor/QueryExtractor.h"
#include "component/QueryProcessor/types/Types.h"

bool AreSynonymsEqual(Synonym s1, Synonym s2) {
  return s1.GetType() == s2.GetType() && s1.GetName() == s2.GetName();
}

bool AreGroupsEqual(Group g1, Group g2) {
  if (g1.ContainsTargetSynonym() != g2.ContainsTargetSynonym()) {
    return false;
  }
  // validate clauses
  auto c1 = g1.GetClauses(); auto c2 = g2.GetClauses();
  auto it1 = c1.begin(); auto it2 = c2.begin();
  while (it1 != c1.end() && it2 != c2.end()) {
    auto _c1 = *it1; auto _c2 = *it2;
    REQUIRE(_c1->isEqual(*_c2));
    it1++; it2++;
  }
  return true;
}

// Queries without 'such that' and 'pattern'
TEST_CASE("3.QueryExtractor.Extract single synonym + select declared synonym; should PASS") {
  std::string query = "assign a1; Select a1";

  auto query_extractor = QueryExtractor(&query);
  query_extractor.ExtractQuery();
  std::list<Synonym> synonyms = query_extractor.GetSynonymsList();
  Synonym target = query_extractor.GetTarget();
  std::list<Group> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
  Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
  std::list<Group> expected_groups;
  REQUIRE(AreSynonymsEqual(expected_target, target) == 1);
  REQUIRE(expected_groups.size() == groups.size());
  auto it1 = expected_synonyms.begin(); auto it2 = synonyms.begin();
  while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
    REQUIRE(AreSynonymsEqual(*it1, *it2) == 1);
    it1++; it2++;
  }
}

TEST_CASE("3.QueryExtractor.Extract single synonym + select undeclared synonym; should FAIL") {
  std::string query = "assign a1; Select a";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect target synonym"));
}

TEST_CASE("3.QueryExtractor.Extract multiple unique synonym + select declared synonym; should PASS") {
  std::string query = "assign a1; procedure p1; Select a1";

  auto query_extractor = QueryExtractor(&query);
  query_extractor.ExtractQuery();
  std::list<Synonym> synonyms = query_extractor.GetSynonymsList();
  Synonym target = query_extractor.GetTarget();
  std::list<Group> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign),
                                            Synonym("p1", DesignEntity:: kProcedure)};
  Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
  std::list<Group> expected_groups;
  REQUIRE(AreSynonymsEqual(expected_target, target) == 1);
  REQUIRE(expected_groups.size() == groups.size());
  auto it1 = expected_synonyms.begin(); auto it2 = synonyms.begin();
  while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
    REQUIRE(AreSynonymsEqual(*it1, *it2) == 1);
    it1++; it2++;
  }
}

TEST_CASE("3.QueryExtractor.Extract multiple unique synonym + select undeclared synonym; should FAIL") {
  std::string query = "assign a1; procedure p1; Select p2";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect target synonym"));
}

// Queries with 1 'such that'
TEST_CASE("3.QueryExtractor.Single malformed such that with typo; should FAIL") {
  std::string query = "assign a; while w; Select a Such that Follows (w, a)";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect clause"));
}

TEST_CASE("3.QueryExtractor.Single malformed such that with extra delimiters; should FAIL") {
  std::string query = "assign a; while w; Select a such  that Follows (w, a)";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect clause"));
}

TEST_CASE("3.QueryExtractor.Single well-formed such that with incorrect relRef; should FAIL") {
  std::string query = "assign a; while w; Select a such that Foll0ws (w, a)";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect"));
}

TEST_CASE("3.QueryExtractor.Single well-formed such that with correct relRef but incorrect left argument; should FAIL") {
  std::string query = "assign a; while w; Select a such that Follows (w1, a)";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect"));
}

TEST_CASE("3.QueryExtractor.Single well-formed such that with correct relRef but incorrect right argument; should FAIL") {
  std::string query = "assign a; while w; Select a such that Follows (w, a1)";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect"));
}
*/

TEST_CASE("3.QueryExtractor.Single well-formed such that with correct relRef and correct argument; should PASS") {
  std::string query = "assign a; while w; Select a such that Follows (w, a)";

  auto query_extractor = QueryExtractor(&query);
  query_extractor.ExtractQuery();
  std::list<Synonym> synonyms = query_extractor.GetSynonymsList();
  Synonym target = query_extractor.GetTarget();
  std::list<Group> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                          Synonym("w", DesignEntity:: kWhile)};
  Synonym expected_target = Synonym("a", DesignEntity::kAssign);
  Clause* cl = new SuchThat("w", "a", RelRef::kFollows, true, true);
  std::vector<Clause*> clauses;
  clauses.push_back(cl);
  std::list<Group> expected_groups;
  Group expected_group = Group(clauses, true);
  expected_groups.push_back(expected_group);

  // validate target
  REQUIRE(AreSynonymsEqual(expected_target, target) == 1);
  // validate synonyms
  auto it1 = expected_synonyms.begin(); auto it2 = synonyms.begin();
  while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
    REQUIRE(AreSynonymsEqual(*it1, *it2) == 1);
    it1++; it2++;
  }
  // validate groups
  REQUIRE(expected_groups.size() == groups.size());
  auto it3 = expected_groups.begin(); auto it4 = groups.begin();
  while (it3 != expected_groups.end() && it4 != groups.end()) {
    REQUIRE(AreGroupsEqual(*it3, *it4) == 1);
    it3++; it4++;
  }
  delete cl;
}


// 'pattern'