//
// Created by Hemanshu Gandhi on 8/9/21.
//

#include "catch.hpp"
#include "component/QueryProcessor/QueryExtractor/QueryExtractor.h"

bool AreSynonymsEqual(Synonym s1, Synonym s2) {
  return s1.GetType() == s2.GetType() && s1.GetName() == s2.GetName();
}

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
