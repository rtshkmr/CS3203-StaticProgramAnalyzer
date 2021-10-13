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
QueryEvaluator::QueryEvaluator(PKB *pkb) : pkb{pkb}, boolean_result{true} {}

UnformattedQueryResult QueryEvaluator::EvaluateQuery(const std::vector<Group>& list_of_groups) {
  UnformattedQueryResult unformatted_result = UnformattedQueryResult(true);

  // REFACTORING IN PROGRESS: new code below
  for (Group current_group : list_of_groups) {
    if (current_group.ContainsTargetSynonym()) {
      // Evaluate non-boolean group
      Synonym first_target_synonym = current_group.GetTargetSynonyms()[0];
      // TODO: Check if this table creation is correct??
      QueryEvaluatorTable *table = new QueryEvaluatorTable(&first_target_synonym);
      DesignEntity de = first_target_synonym.GetType();

      table->AddTargetSynonymValues(pkb->GetDesignEntities(de));
      ProcessGroup(table, current_group);
      unformatted_result.AddTable(table);
    } else {
      // Evaluate boolean group
      PreprocessBooleanGroup(current_group);
      if (boolean_result) {
        continue;
      }
      unformatted_result.SetBooleanResult(false);
      break;
    }
  }

  return unformatted_result;
  // REFACTORING IN PROGRESS: end ^
}

/**
 * Determines the type of strategy for each clause and calls the respective clause executor to compute the new
 * intermediate table.
 * @param table
 * @param group
 */
void QueryEvaluator::ProcessGroup(QueryEvaluatorTable *table, Group group) {

  for (Clause* current_clause: group.GetClauses()) {
    ClauseContext clause_context = ClauseContext(table);
    std::tuple<PKBQueryCommand*, ClauseCommand*> commands = clause_context.ProcessClause(current_clause);
    PKBQueryCommand *query_command = std::get<0>(commands);
    ClauseCommand *clause_command = std::get<1>(commands);
    IntermediateTable *intermediate_table = query_command->ExecuteQuery(current_clause);

    ClauseCommandExecutor clause_executor = ClauseCommandExecutor(table, intermediate_table);
    clause_command->SetExecutor(&clause_executor);
    clause_command->Execute(current_clause);
  }
  return;
}

bool QueryEvaluator::ProcessSingleClauseBooleanGroup(Group group) {
  Clause *clause = group.GetClauses()[0];
  auto *such_that_clause = dynamic_cast<SuchThat *>(clause);
  QuerySuchThatNoSynonymCommand query_command = QuerySuchThatNoSynonymCommand(clause);
  // TODO: NOT SURE HOW TO QUERY PKB

  return true;
}

/**
 * Finds the main synonym for a boolean group and adding it to the table representing the current boolean group.
 * @param clause_list The list of clause in the current group.
 */
void QueryEvaluator::PreprocessBooleanGroup(Group group) {
  Clause* firstClause = group.GetClauses()[0];
  Synonym *main_synonym = nullptr;
  if (typeid(* firstClause) == typeid(SuchThat)) {
    auto* st = dynamic_cast<SuchThat*>(firstClause);
    if (st->left_is_synonym) {
      main_synonym = &st->first_synonym;
    } else if (st->right_is_synonym) {
      main_synonym = &st->second_synonym;
    } else {
      // If no synonym, no need for a table. Also, should be in a group of size 1.
      // TODO: add function for no syn
      boolean_result = ProcessSingleClauseBooleanGroup(group);
      return;
    }
  } else if (typeid(* firstClause) == typeid(Pattern)) {
    auto* pattern = dynamic_cast<Pattern*>(firstClause);
    main_synonym = &pattern->first_synonym;
  } else {
    // No code should run here for iter 1 since there is only such that and pattern clause.
  }

  if (main_synonym != nullptr) {
    QueryEvaluatorTable current_table(main_synonym);
    current_table.AddTargetSynonymValues(pkb->GetDesignEntities(main_synonym->GetType()));
    ProcessGroup(&current_table, group);
    if (current_table.GetResults().empty()) {
      boolean_result = false;
    }
  }
  return;
}
