#include "catch.hpp"
#include "component/QueryProcessor/QueryEvaluator/ClauseStrategy.h"

class MockQueryEvaluatorTable : public QueryEvaluatorTable {
 private:
  bool has_synonym;
 public:
  MockQueryEvaluatorTable(Synonym* target, bool has_synonym) : QueryEvaluatorTable(target), has_synonym(has_synonym) {}

  bool ContainsColumn(Synonym* synonym) override {
    return has_synonym;
  }
};

// Note that the only public method is ClauseContext::ProcessClause()



TEST_CASE("3.ClauseStrategy.Such That Double Synonym Both In") {
  QueryEvaluatorTable* table = new MockQueryEvaluatorTable(nullptr, true);
  Clause* current_clause = new SuchThat("a1", "p1", RelRef::kFollowsT, true, true);

  auto context = ClauseContext(table);
  std::tuple<PKBQueryCommand*, ClauseCommand*> commands = context.ProcessClause(current_clause);
  PKBQueryCommand* query_command = std::get<0>(commands);
  bool outcome = typeid(* query_command) == typeid(QuerySuchThatTwoSynonymCommand);
  REQUIRE(outcome);
  ClauseCommand* clause_command = std::get<1>(commands);
  outcome = typeid(* clause_command) == typeid(DoubleSynonymBothPresentCommand);
  REQUIRE(outcome);
}

TEST_CASE("3.ClauseStrategy.Such That Double Synonym Both Not In") {
  QueryEvaluatorTable* table = new MockQueryEvaluatorTable(nullptr, false);
  Clause* current_clause = new SuchThat("a1", "p1", RelRef::kFollowsT, true, true);

  auto context = ClauseContext(table);
  std::tuple<PKBQueryCommand*, ClauseCommand*> commands = context.ProcessClause(current_clause);
  PKBQueryCommand* query_command = std::get<0>(commands);
  bool outcome = typeid(* query_command) == typeid(QuerySuchThatTwoSynonymCommand);
  REQUIRE(outcome);
  ClauseCommand* clause_command = std::get<1>(commands);
  outcome = typeid(* clause_command) == typeid(DoubleSynonymNonePresentCommand);
  REQUIRE(outcome);
}

TEST_CASE("3.ClauseStrategy.Such That Single Synonym In Table") {
  QueryEvaluatorTable* table = new MockQueryEvaluatorTable(nullptr, true);
  auto context = ClauseContext(table);

  SECTION("First param is synonym") {
    Clause* current_clause = new SuchThat("a1", "p1", RelRef::kFollowsT, true, false);
    std::tuple<PKBQueryCommand*, ClauseCommand*> commands = context.ProcessClause(current_clause);
    PKBQueryCommand* query_command = std::get<0>(commands);
    bool outcome = typeid(* query_command) == typeid(QuerySuchThatOneSynonymCommand);
    REQUIRE(outcome);
    ClauseCommand* clause_command = std::get<1>(commands);
    outcome = typeid(* clause_command) == typeid(SingleSynonymPresentCommand);
    REQUIRE(outcome);
  }

  SECTION("Second param is synonym") {
    Clause* current_clause = new SuchThat("a1", "p1", RelRef::kFollowsT, false, true);
    std::tuple<PKBQueryCommand*, ClauseCommand*> commands = context.ProcessClause(current_clause);
    PKBQueryCommand* query_command = std::get<0>(commands);
    bool outcome = typeid(* query_command) == typeid(QuerySuchThatOneSynonymCommand);
    REQUIRE(outcome);
    ClauseCommand* clause_command = std::get<1>(commands);
    outcome = typeid(* clause_command) == typeid(SingleSynonymPresentCommand);
    REQUIRE(outcome);
  }
}

TEST_CASE("3.ClauseStrategy.Such That No Synonym") {
  QueryEvaluatorTable* table = new MockQueryEvaluatorTable(nullptr, false);
  auto context = ClauseContext(table);

  Clause* current_clause = new SuchThat("a1", "p1", RelRef::kFollowsT, false, false);
  std::tuple<PKBQueryCommand*, ClauseCommand*> commands = context.ProcessClause(current_clause);
  PKBQueryCommand* query_command = std::get<0>(commands);
  bool outcome = typeid(* query_command) == typeid(QuerySuchThatNoSynonymCommand);
  REQUIRE(outcome);
  ClauseCommand* clause_command = std::get<1>(commands);
  outcome = clause_command == nullptr;
  REQUIRE(outcome);
}
