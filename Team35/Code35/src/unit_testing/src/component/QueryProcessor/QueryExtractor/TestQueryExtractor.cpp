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
  std::vector<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
  Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
  std::vector<Group*> expected_groups;
  std::vector<Clause*> no_clauses;
  Group g = Group(no_clauses, true);
  expected_groups.push_back(&g);
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
  std::vector<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                          Synonym("b", DesignEntity::kAssign),
                                          Synonym("c", DesignEntity::kAssign)};
  Synonym expected_target = Synonym("a", DesignEntity::kAssign);
  std::vector<Group*> expected_groups;
  std::vector<Clause*> no_clauses;
  Group g = Group(no_clauses, true);
  expected_groups.push_back(&g);
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
  Synonym target = *query_extractor.GetTargetSynonymsList().at(0);
  std::vector<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign),
                                          Synonym("p1", DesignEntity::kProcedure)};
  Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
  std::vector<Group*> expected_groups;
  std::vector<Clause*> no_clauses;
  Group g = Group(no_clauses, true);
  expected_groups.push_back(&g);
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

TEST_CASE("3.QueryExtractor.Extract multiple synonyms + select BOOLEAN; should PASS") {
  std::string query = "assign a1, a2, a3, a4, a5; Select BOOLEAN";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  std::vector<Synonym*> target_synonyms = query_extractor.GetTargetSynonymsList();

  // we have already tested that parsing of declaration synonyms works, so focus on checking target_synonyms_list
  REQUIRE(synonyms.size() == 5);
  REQUIRE(groups.size() == 0);
  REQUIRE(target_synonyms.size() == 0);
}

TEST_CASE("3.QueryExtractor.Extract multiple synonyms + select tuple of declared synonyms; should PASS") {
  std::string query = "assign a1, a2, a3, a4, a5; Select <a2, a1, a3, a4, a5>";

  auto query_extractor = QueryExtractor(& query);
  query_extractor.ExtractQuery();
  std::list<Synonym*> synonyms = query_extractor.GetSynonymsList();
  std::vector<Group*> groups = query_extractor.GetGroupsList();
  std::vector<Synonym*> target_synonyms = query_extractor.GetTargetSynonymsList();

  // we have already tested that parsing of declaration synonyms works, so focus on checking target_synonyms_list
  REQUIRE(synonyms.size() == 5);
  REQUIRE(groups.size() == 5);

  std::vector<Synonym*> expected_target_synonyms = {new Synonym("a2", DesignEntity::kAssign),
                                                    new Synonym("a1", DesignEntity::kAssign),
                                                    new Synonym("a3", DesignEntity::kAssign),
                                                    new Synonym("a4", DesignEntity::kAssign),
                                                    new Synonym("a5", DesignEntity::kAssign)};
  REQUIRE(target_synonyms.size() == 5);
  for (int i = 0; i < expected_target_synonyms.size(); i++) {
    REQUIRE(AreSynonymsEqual(*target_synonyms[i], *expected_target_synonyms[i]));
  }
}

TEST_CASE("3.QueryExtractor.Single malformed such that with typo; should FAIL") {
  std::string query = "assign a; while w; Select a Such that Follows (w, a)";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect query."));
}

TEST_CASE("3.QueryExtractor.Single malformed such that with extra delimiters; should FAIL") {
  std::string query = "assign a; while w; Select a such  that Follows (w, a)";
  auto query_extractor = QueryExtractor(&query);
  REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(), Catch::Contains("Incorrect query."));
}

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
  std::vector<Group*> groups = query_extractor.GetGroupsList();

  std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                          Synonym("w", DesignEntity::kWhile)};
  Synonym expected_target = Synonym("a", DesignEntity::kAssign);
  Clause* cl = new SuchThat("w", "a", RelRef::kFollows, true, true);
  std::vector<Clause*> clauses;
  clauses.push_back(cl);
  std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("a", DesignEntity::kAssign);
    Clause* cl = new SuchThat("a", "x", RelRef::kUsesS, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a", DesignEntity::kAssign);
    Clause* cl = new SuchThat("a", "_", RelRef::kUsesS, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("p", DesignEntity::kProcedure),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("p", DesignEntity::kProcedure);
    Clause* cl = new SuchThat("p", "x", RelRef::kUsesP, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("c", DesignEntity::kCall),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("c", DesignEntity::kCall);
    Clause* cl = new SuchThat("c", "x", RelRef::kUsesP, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a", DesignEntity::kAssign),
                                            Synonym("v", DesignEntity::kVariable)};
    Synonym expected_target = Synonym("a", DesignEntity::kAssign);
    Clause* cl = new SuchThat("main", "x", RelRef::kUsesP, false, false);
    std::vector<Clause*> clauses;
    std::vector<Clause*> empty_clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
    Group expected_group_1 = Group(empty_clauses, true);
    Group expected_group_2 = Group(clauses, false);
    expected_groups.push_back(& expected_group_1);
    expected_groups.push_back(& expected_group_2);

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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "_", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "69", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "count", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "v + x * y + z * t", "a1", false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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
    std::vector<Group*> groups = query_extractor.GetGroupsList();

    std::list<Synonym> expected_synonyms = {Synonym("a1", DesignEntity::kAssign)};
    Synonym expected_target = Synonym("a1", DesignEntity::kAssign);
    Clause* cl = new Pattern("_", "v + x * y + z * t", "a1", false, true);
    std::vector<Clause*> clauses;
    clauses.push_back(cl);
    std::vector<Group*> expected_groups;
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

// negative testing for 'and' operator between multiple clauses
TEST_CASE("3.QueryExtractor.And operator between different clause types; should FAIL") {
  SECTION("And between relCond and rel") { // because pql grammar only supports 'and' between relCond and relCond.
    std::string query = "assign a; while w;"
                        "Select a such that Parent* (w, a) and Modifies (a, \"x\") and such that Uses (a, \"x\")";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Invalid relRef in such that clause."));
  }

  SECTION("And between relCond and pattern") {
    std::string query = "assign a; while w;"
                        "Select a such that Parent* (w, a) and pattern a (\"x\", _) such that Uses (a, \"x\")";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Invalid relRef in such that clause."));
  }

  SECTION("And between patternCond and relCond") {
    std::string query = "assign a; while w;"
                        "Select a such that Parent* (w, a) pattern a (\"x\", _) and Uses (a, \"x\")";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Expected valid syn-assign for pattern cl, instead got"));
  }

  SECTION("And between patternCond and pattern") {
    std::string query = "assign a, a2; while w;"
                        "Select a such that Parent* (w, a) pattern a (\"x\", _) and pattern a2 (\"x\", _)";
    auto query_extractor = QueryExtractor(& query);
    REQUIRE_THROWS_WITH(query_extractor.ExtractQuery(),
                        Catch::Contains("Expected valid syn-assign for pattern cl, instead got"));
  }
}

// advanced relationships
TEST_CASE("3.QueryExtractor.Single well-formed such that Calls; should pass") {
  SECTION("Calls with lhs proc synonym + rhs IDENT") {
    std::string query = "procedure p; Select p such that Calls(p, \"q\")";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 1);
    Group* actual_group = actual_groups[0];

    Clause* expected_cl = new SuchThat("p", "q", RelRef::kCalls, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, true);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Calls* with lhs IDENT + rhs proc synonym") {
    std::string query = "procedure p; Select p such that Calls*(\"Example\", p)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 1);
    Group* actual_group = actual_groups[0];

    Clause* expected_cl = new SuchThat("Example", "p", RelRef::kCallsT, false, true);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, true);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Calls with lhs IDENT + rhs IDENT") {
    std::string query = "procedure p; Select p such that Calls(\"First\", \"Second\")";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 2);
    Group* actual_group = actual_groups[1];

    Clause* expected_cl = new SuchThat("First", "Second", RelRef::kCalls, false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, false);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Calls* with lhs proc synonym + rhs proc synonym") {
    std::string query = "procedure p, q; Select p such that Calls*(p, q)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 1);
    Group* actual_group = actual_groups[0];

    Clause* expected_cl = new SuchThat("p", "q", RelRef::kCallsT, true, true);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, true);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }
}

TEST_CASE("3.QueryExtractor.Single well-formed such that Next; should pass") {
  SECTION("Next with lhs INTEGER + rhs INTEGER") {
    std::string query = "procedure p; Select p such that Next(2, 3)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 2);
    Group* actual_group = actual_groups[1];

    Clause* expected_cl = new SuchThat("2", "3", RelRef::kNext, false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, false);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Next* containing prog_line syn + INTEGER") {
    std::string query = "assign a; prog_line n; Select a such that Next* (60, n)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 2);
    Group* actual_group = actual_groups[1];

    Clause* expected_cl = new SuchThat("60", "n", RelRef::kNextT, false, true);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, false);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Next containing generic stmt syn + INTEGER") {
    std::string query = "assign a; stmt n; Select a such that Next (60, n)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 2);
    Group* actual_group = actual_groups[1];

    Clause* expected_cl = new SuchThat("60", "n", RelRef::kNext, false, true);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, false);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Next* containing stmt subtype syn + INTEGER") {
    // assign is an example of a syn that is considered a type of stmt
    std::string query = "assign a; Select a such that Next* (60, a)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 1);
    Group* actual_group = actual_groups[0];

    Clause* expected_cl = new SuchThat("60", "a", RelRef::kNextT, false, true);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, true);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }
}

TEST_CASE("3.QueryExtractor.Single well-formed such that Affects; should pass") {
  SECTION("Affects with lhs INTEGER + rhs INTEGER") {
    std::string query = "assign a; Select a such that Affects(2, 3)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 2);
    Group* actual_group = actual_groups[1];

    Clause* expected_cl = new SuchThat("2", "3", RelRef::kAffects, false, false);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, false);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Affects* containing assign syn + INTEGER") {
    std::string query = "assign a; Select a such that Affects* (a, 10)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 1);
    Group* actual_group = actual_groups[0];

    Clause* expected_cl = new SuchThat("a", "10", RelRef::kAffectsT, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, true);;

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }

  SECTION("Affects* containing generic stmt syn + INTEGER") {
    std::string query = "stmt s; Select s such that Affects* (s, 10)";
    auto query_extractor = QueryExtractor(& query);
    query_extractor.ExtractQuery();
    std::vector<Group*> actual_groups = query_extractor.GetGroupsList();
    REQUIRE(actual_groups.size() == 1);
    Group* actual_group = actual_groups[0];

    Clause* expected_cl = new SuchThat("s", "10", RelRef::kAffectsT, true, false);
    std::vector<Clause*> clauses;
    clauses.push_back(expected_cl);
    Group* expected_group = new Group(clauses, true);

    REQUIRE(AreGroupsEqual(expected_group, actual_group));
  }
}