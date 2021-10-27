#include <util/Logger.h>

#include <utility>
#include "QueryEvaluator.h"
#include "ClauseCommandExecutor.h"
#include "ClauseStrategy.h"

/**
 * Processes the group list containing the information of the query. This method will then make the relevant calls to
 * various methods to further process each group of queries.
 * @param pkb The populated PKB database.
 */
QueryEvaluator::QueryEvaluator(DBManager *db_manager) : db_manager{db_manager}, boolean_result{true} {}

UnformattedQueryResult QueryEvaluator::EvaluateQuery(const std::vector<Group *>& list_of_groups) {
  UnformattedQueryResult unformatted_result = UnformattedQueryResult(true);

  for (Group *current_group : list_of_groups) {
    if (current_group->ContainsTargetSynonym()) {
      // Evaluate non-boolean group
      auto *table = new QueryEvaluatorTable(current_group->GetTargetSynonyms());
      PreprocessNonBooleanGroup(current_group, table);
      unformatted_result.AddTable(table);
    } else {
      // Evaluate boolean group
      PreprocessBooleanGroup(current_group);
      if (!boolean_result) {
        unformatted_result.SetBooleanResult(false);
        break;
      }
    }
  }
  return unformatted_result;
}

/**
 * Determines the type of strategy for each clause and calls the respective clause executor to compute the new
 * intermediate table.
 * @param table
 * @param group
 */
void QueryEvaluator::ProcessGroup(QueryEvaluatorTable *table, Group *group) {
  for (Clause* current_clause: group->GetClauses()) {
    auto clause_context = ClauseContext(table);
    std::tuple<PKBQueryCommand*, ClauseCommand*> commands = clause_context.ProcessClause(current_clause);
    PKBQueryCommand *query_command = std::get<0>(commands);
    ClauseCommand *clause_command = std::get<1>(commands);

    auto query_receiver = PKBQueryReceiver(db_manager);
    query_command->SetReceiver(&query_receiver);
    IntermediateTable *intermediate_table = query_command->ExecuteQuery(current_clause);

    ClauseCommandExecutor clause_executor = ClauseCommandExecutor(table, intermediate_table);
    clause_command->SetExecutor(&clause_executor);
    clause_command->Execute(current_clause);
  }
  return;
}

bool QueryEvaluator::ProcessBooleanGroupWithoutSynonym(Group *group) {
  Clause *clause = group->GetClauses()[0];
  // Only such that clause can have a single boolean group.
  auto query_command = QuerySuchThatNoSynonymCommand(clause);
  auto query_receiver = PKBQueryReceiver(db_manager);
  query_command.SetReceiver(&query_receiver);
  IntermediateTable *table = query_command.ExecuteQuery(clause);
  return table->GetExistenceResult();
}

/**
 * Finds the main synonym for a boolean group and adding it to the table representing the current boolean group.
 * @param clause_list The list of clause in the current group.
 */
void QueryEvaluator::PreprocessBooleanGroup(Group *group) {
  Clause* firstClause = group->GetClauses()[0];
  Synonym *main_synonym = nullptr;
  if (typeid(* firstClause) == typeid(SuchThat)) {
    main_synonym = ProcessMainSynonymFromSuchThat(firstClause, group);
  } else if (typeid(* firstClause) == typeid(Pattern)) {
    main_synonym = ProcessMainSynonymFromPattern(firstClause);
  } else {
    // No code should run here for iter 1 since there is only such that and pattern clause.
  }

  if (main_synonym != nullptr) {
    ProcessBooleanGroupWithSynonym(group, main_synonym);
  }
  return;
}

void QueryEvaluator::ProcessBooleanGroupWithSynonym(Group *group, Synonym *main_synonym) {
  QueryEvaluatorTable current_table(main_synonym);
  auto entity_list_test = db_manager->GetDesignEntities(main_synonym->GetType());
  current_table.AddTargetSynonymValues(main_synonym, entity_list_test);
  ProcessGroup(&current_table, group);
  if (current_table.GetResults()[0].empty()) {
    boolean_result = false;
  }
}

Synonym *QueryEvaluator::ProcessMainSynonymFromSuchThat(Clause *first_clause, Group *group) {
  auto* st = dynamic_cast<SuchThat*>(first_clause);
  if (st->left_is_synonym) {
    return st->first_synonym;
  } else if (st->right_is_synonym) {
    return st->second_synonym;
  } else {
    // If no synonym, no need for a table. Also, should be in a group of size 1.
    boolean_result = ProcessBooleanGroupWithoutSynonym(group);
    return nullptr;
  }
}

Synonym *QueryEvaluator::ProcessMainSynonymFromPattern(Clause *first_clause) {
  auto* pattern = dynamic_cast<Pattern*>(first_clause);
  return pattern->first_synonym;
}

void QueryEvaluator::PreprocessNonBooleanGroup(Group *group, QueryEvaluatorTable *table) {
  Synonym *first_target_synonym = group->GetTargetSynonyms()[0];
  DesignEntity de = first_target_synonym->GetType();

  table->AddTargetSynonymValues(first_target_synonym, db_manager->GetDesignEntities(de));
  ProcessGroup(table, group);
}
