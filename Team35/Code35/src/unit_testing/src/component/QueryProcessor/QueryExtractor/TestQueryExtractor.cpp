#include "catch.hpp"
#include "component/QueryProcessor/QueryExtractor/QueryExtractor.h"
#include "component/QueryProcessor/types/Types.h"

bool AreSynonymsEqual(Synonym s1, Synonym s2) {
  return s1.GetType() == s2.GetType() && s1.GetName() == s2.GetName();
}

bool AreGroupsEqual(Group* g1, Group* g2) {
  if (g1->ContainsTargetSynonym() != g2->ContainsTargetSynonym()) {
    return false;
  }
  // validate clauses
  auto c1 = g1->GetClauses();
  auto c2 = g2->GetClauses();
  auto it1 = c1.begin();
  auto it2 = c2.begin();
  while (it1 != c1.end() && it2 != c2.end()) {
    auto _c1 = * it1;
    auto _c2 = * it2;
    REQUIRE(_c1->isEqual(* _c2));
    it1++;
    it2++;
  }
  return true;
}

// Queries without 'such that' and 'pattern'

TEST_CASE("3.QueryExtractor.Extract single synonym + select declared synonym; should PASS") {
  std::string query = "assign a1; Select a1";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
  Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
  std::list<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
  Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
  std::list<Group*> expected_groups;
  REQUIRE(AreSynonymsEqual(expected_target, target));
  REQUIRE(expected_groups.size() == groups.size());
  auto it1 = expected_synonyms.begin();
  auto it2 = synonyms.begin();
  while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
    REQUIRE(AreSynonymsEqual(* it1, ** it2));
    it1++;
    it2++;
  }
}

TEST_CASE("3.QueryExtractor.Extract multiple synonym + select declared synonym; should PASS") {
  std::string query = "assign a, b, c; Select a";
  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
  Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
  std::list<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                          Synonym("b", DesignEntity::kAssign),
                                          Synonym("c", DesignEntity::kAssign)};
  Synonym expected_target = Synonym("a", DesignEntity::kAssign);
  std::list<Group*> expected_groups;
  REQUIRE(AreSynonymsEqual(expected_target, target));
  REQUIRE(expected_groups.size() == groups.size());
  auto it1 = expected_synonyms.begin();
  auto it2 = synonyms.begin();
  while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
    REQUIRE(AreSynonymsEqual(* it1, ** it2));
    it1++;
    it2++;
  }
}

TEST_CASE("3.QueryExtractor.Extract single synonym + select undeclared synonym; should FAIL") {
  std::string query = "assign a1; Select a";
  auto query_extractor = QueryExtractor(& query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect target synonym"));
}

TEST_CASE("3.QueryExtractor.Extract multiple unique synonym + select declared synonym; should PASS") {
  std::string query = "assign a1; procedure p1; Select a1";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
  Synonym target = &query_extractor.GetTargetSynonymsList().at(0);
  std::list<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign),
                                          Synonym("p1", DesignEntity::kProcedure)};
  Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
  std::list<Group*> expected_groups;
  REQUIRE(AreSynonymsEqual(expected_target, target));
  REQUIRE(expected_groups.size() == groups.size());
  auto it1 = expected_synonyms.begin();
  auto it2 = synonyms.begin();
  while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
    REQUIRE(AreSynonymsEqual(* it1, ** it2));
    it1++;
    it2++;
  }
}

TEST_CASE("3.QueryExtractor.Extract multiple unique synonym + select undeclared synonym; should FAIL") {
  std::string query = "assign a1; procedure p1; Select p2";
  auto query_extractor = QueryExtractor(& query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect target synonym"));
}

// Temporarily commented out due to change in QueryParser.cpp (lines 183 to 188)
//// Queries with 1 'such that'
//TEST_CASE("3.QueryExtractor.Single malformed such that with typo; should FAIL") {
//  std::string query = "assign a; while w; Select a Such that Follows (w, a)";
//  auto query_extractor = QueryExtractor(&query);
//  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect query."));
//}
//
//TEST_CASE("3.QueryExtractor.Single malformed such that with extra delimiters; should FAIL") {
//  std::string query = "assign a; while w; Select a such  that Follows (w, a)";
//  auto query_extractor = QueryExtractor(&query);
//  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect query."));
//}

TEST_CASE("3.QueryExtractor.Single well-formed such that with incorrect relRef; should FAIL") {
  std::string query = "assign a; while w; Select a such that Foll0ws (w, a)";
  auto query_extractor = QueryExtractor(& query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Invalid relRef"));
}

TEST_CASE("3.QueryExtractor.Single well-formed such that with correct relRef but incorrect left argument; should FAIL") {
  std::string query = "assign a; while w; Select a such that Follows (w1, a)";
  auto query_extractor = QueryExtractor(& query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Unknown synonym supplied"));
}

TEST_CASE("3.QueryExtractor.Single well-formed such that with correct relRef but incorrect right argument; should FAIL") {
  std::string query = "assign a; while w; Select a such that Follows (w, a1)";
  auto query_extractor = QueryExtractor(& query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Unknown synonym supplied"));
}

TEST_CASE("3.QueryExtractor.Single well-formed such that with correct relRef and correct argument; should PASS") {
  std::string query = "assign a; while w; Select a such that Follows (w, a)";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
  Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
  std::list<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                          Synonym("w", DesignEntity::kWhile)};
  Synonym expected_target = Synonym("a", DesignEntity::kAssign);
  Clause* cl = new SuchThat("w", "a", RelRef::kFollows, true, true);
  std::vector<Clause*> clauses;
  clauses.push_back(cl);
  std::list<Group*> expected_groups;
  Group expected_group = Group(clauses, true);
  expected_groups.push_back(& expected_group);

  // validate target
  REQUIRE(AreSynonymsEqual(expected_target, target));
  // validate synonyms
  auto it1 = expected_synonyms.begin();
  auto it2 = synonyms.begin();
  while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
    REQUIRE(AreSynonymsEqual(* it1, ** it2));
    it1++;
    it2++;
  }
  // validate groups
  REQUIRE(expected_groups.size() == groups.size());
  auto it3 = expected_groups.begin();
  auto it4 = groups.begin();
  while (it3 != expected_groups.end() && it4 != groups.end()) {
    REQUIRE(AreGroupsEqual(* it3, * it4));
    it3++;
    it4++;
  }
  delete cl;
}

TEST_CASE("3.QueryExtractor.Single well-formed such that with correct entRef and correct argument; should PASS") {
  SECTION("entRef is string IDENT") {
    std::string query = "assign a; variable v; Select a such that Uses(a, \"x\")";

    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("a", DesignEntity::kAssign);
    Clause* cl = new SuchThat("a", "x", RelRef::kUsesS, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }

  SECTION("entRef is underscore") {
    std::string query = "assign a; Select a such that Uses(a, _)";

    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a", DesignEntity::kAssign);
    Clause* cl = new SuchThat("a", "_", RelRef::kUsesS, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }
}

// since the rhs of UsesS/ModifiesS is already entref, we have already tested that it parses correctly in above tests.
// hence, to check that UsesP/ModifiesP parses correctly, we only need additional checks for lhs.
TEST_CASE("3.QueryExtractor.Single well-formed UsesP or ModifiesP; should PASS") {
  SECTION("lhs is SYNONYM of type procedure") {
    std::string query = "procedure p; variable v; Select p such that Uses(p, \"x\")";

    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("p", DesignEntity::kProcedure),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("p", DesignEntity::kProcedure);
    Clause* cl = new SuchThat("p", "x", RelRef::kUsesP, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }

  SECTION("lhs is SYNONYM of type call") {
    std::string query = "call c; variable v; Select c such that Uses(c, \"x\")";

    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("c", DesignEntity::kCall),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("c", DesignEntity::kCall);
    Clause* cl = new SuchThat("c", "x", RelRef::kUsesP, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }

  SECTION("lhs is string IDENT") {
    std::string query = "assign a; variable v; Select a such that Uses(\"main\", \"x\")";

    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("a", DesignEntity::kAssign);
    Clause* cl = new SuchThat("main", "x", RelRef::kUsesP, false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, false);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }
}

// 'pattern'
TEST_CASE("3.QueryExtractor.Single malformed pattern with typo; should FAIL") {
  std::string query = "assign a1; Select a1 pAttern a ( _ , _)";
  auto query_extractor = QueryExtractor(& query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect query"));
}

TEST_CASE("3.QueryExtractor.Single well-formed pattern with unknown syn-assign; should FAIL") {
  std::string query = "assign a1; Select a1 pattern a ( _ , _)";
  auto query_extractor = QueryExtractor(& query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                      Catch::Contains("Expected valid syn-assign for pattern cl, instead got"));
}

TEST_CASE("3.QueryExtractor.Single well-formed pattern with correct syn-assign but incorrect lhs; should FAIL") {
  SECTION("lhs is unknown synonym") {
    std::string query = "assign a1; Select a1 pattern a1 ( p , _)";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Unknown synonym received as entRef in lhs of pattern cl."));
  }
  SECTION("lhs is known synonym but not of type variable") {
    std::string query = "assign a1; Select a1 pattern a1 ( a1 , _)";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Unknown synonym received as entRef in lhs of pattern cl."));
  }
  SECTION("lhs is a string but not ident") {
    std::string query = "assign a1; Select a1 pattern a1 (\"69\" , _)";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Unexpected token"));
  }
}

TEST_CASE("3.QueryExtractor.Single well-formed pattern with correct syn-assign but incorrect rhs; should FAIL") {
  SECTION("malformed rhs of partial match") {
    std::string query = "assign a1; Select a1 pattern a1 ( _ , _\"count\")";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Unexpected token"));
  }
  SECTION("malformed lhs of partial match") {
    std::string query = "assign a1; Select a1 pattern a1 ( _ , \"count\"_)";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Unexpected token"));
  }
}

TEST_CASE("3.QueryExtractor.Single well-formed pattern with correct syn-assign and correct args; should PASS") {
  SECTION("rhs is underscore") {
    std::string query = "assign a1; Select a1 pattern a1 ( _ , _)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "_", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }

  SECTION("rhs is partial match with factor string of type const") {
    std::string query = "assign a1; Select a1 pattern a1 ( _ , _\"69\"_)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "69", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }

  SECTION("rhs is partial match with factor string of type variable name") {
    std::string query = "assign a1; Select a1 pattern a1 ( _ , _\"count\"_)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "count", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }

  SECTION("rhs is partial match with complex expression-spec") {
    std::string query = "assign a1; Select a1 pattern a1 ( _ , _\"v + x * y + z * t\"_)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "v + x * y + z * t", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }

  SECTION("rhs is exact match with complex expression-spec") {
    std::string query = "assign a1; Select a1 pattern a1 ( _ , \"v + x * y + z * t\")";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
    Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
    std::list<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "v + x * y + z * t", "a1", false, true);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::list<Group*> expected_groups;
    Group expected_group = Group(clauses, true);
    expected_groups.push_back(& expected_group);

    // validate target
    REQUIRE(AreSynonymsEqual(expected_target, target));
    // validate synonyms
    auto it1 = expected_synonyms.begin();
    auto it2 = synonyms.begin();
    while (it1 != expected_synonyms.end() && it2 != synonyms.end()) {
      REQUIRE(AreSynonymsEqual(* it1, ** it2));
      it1++;
      it2++;
    }
    // validate groups
    REQUIRE(expected_groups.size() == groups.size());
    auto it3 = expected_groups.begin();
    auto it4 = groups.begin();
    while (it3 != expected_groups.end() && it4 != groups.end()) {
      REQUIRE(AreGroupsEqual(* it3, * it4));
      it3++;
      it4++;
    }
    delete cl;
  }
}
