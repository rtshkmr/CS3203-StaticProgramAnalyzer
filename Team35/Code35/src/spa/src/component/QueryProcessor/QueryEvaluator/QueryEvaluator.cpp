#include <cassert>
#include "QueryEvaluator.h"
#include "ClauseCommandExecutor.h"
#include "ClauseStrategy.h"

QueryEvaluator::QueryEvaluator(DBManager* db_manager) : db_manager{db_manager}, boolean_result{true} {}

/**
 * Processes the group list containing the information of the query. This method will then make the relevant calls to
 * various methods to further process each group of queries. After the boolean or non-boolean groups have been processed,
 * the UnformattedQuery
 * @param list_of_groups The list containing boolean and non-boolean groups
 * @return UnformattedQueryResult which contains QueryEvaluatorTable references and a boolean value.
 */
UnformattedQueryResult QueryEvaluator::EvaluateQuery(const std::vector<Group*>& list_of_groups) {
  UnformattedQueryResult unformatted_result = UnformattedQueryResult(true);
  for (Group* current_group: list_of_groups) {
    if (current_group->ContainsTargetSynonym()) {       // Evaluate non-boolean group
      auto* table = new QueryEvaluatorTable(current_group->GetTargetSynonyms());
      PreprocessNonBooleanGroup(current_group, table);
      unformatted_result.AddTable(table);
    } else {                                            // Evaluate boolean group
      PreprocessBooleanGroup(current_group);
    }
    if (!boolean_result) {
      unformatted_result.SetBooleanResult(false);
      break;
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
void QueryEvaluator::ProcessGroup(QueryEvaluatorTable* table, Group* group) {
  for (Clause* current_clause: group->GetClauses()) {
    auto clause_context = ClauseContext(table);
    std::tuple<PKBQueryCommand*, ClauseCommand*> commands = clause_context.ProcessClause(current_clause);
    PKBQueryCommand* query_command = std::get<0>(commands);
    ClauseCommand* clause_command = std::get<1>(commands);

    auto query_receiver = PKBQueryReceiver(db_manager, table);
    query_command->SetReceiver(& query_receiver);
    IntermediateTable* intermediate_table = query_command->ExecuteQuery(current_clause);
    ClauseCommandExecutor clause_executor = ClauseCommandExecutor(table, intermediate_table);
    clause_command->SetExecutor(& clause_executor);
    clause_command->Execute(current_clause);
    if (table->GetRowSize() == 0) break;        // Optimisation through early termination.
  }
}

void QueryEvaluator::ProcessBooleanGroupWithoutSynonym(Group* group) {
  assert(group->GetClauses().size() == 1);
  Clause* clause = group->GetClauses()[0];

  if (typeid(* clause) == typeid(SuchThat)) {
    auto query_command = QuerySuchThatNoSynonymCommand(clause);
    auto query_receiver = PKBQueryReceiver(db_manager);
    query_command.SetReceiver(& query_receiver);
    IntermediateTable* table = query_command.ExecuteQuery(clause);
    boolean_result = table->GetExistenceResult();
  } else {
    assert(typeid(* clause) == typeid(With));
    auto with_clause = dynamic_cast<With*>(clause);
    boolean_result = with_clause->HasEqualValue();
  }
}

/**
 * Finds the main synonym for a boolean group and adding it to the table representing the current boolean group.
 * @param clause_list The list of clause in the current group.
 */
void QueryEvaluator::PreprocessBooleanGroup(Group* group) {
  Clause* firstClause = group->GetClauses()[0];
  Synonym* main_synonym = GetMainSynonymFromClause(firstClause);

  if (main_synonym != nullptr) {
    ProcessBooleanGroupWithSynonym(group, main_synonym);
  } else {
    ProcessBooleanGroupWithoutSynonym(group);
  }
}

/**
 * This method sets up the QueryEvaluatorTable in a similar fashion for the non-boolean groups, then the group is sent
 * to be processed. Afterwards, it checks if the size of the table is 0 to determine if the boolean group holds.
 * @param group
 * @param main_synonym
 */
void QueryEvaluator::ProcessBooleanGroupWithSynonym(Group* group, Synonym* main_synonym) {
  QueryEvaluatorTable current_table(main_synonym);
  auto entity_list_test = db_manager->GetDesignEntities(main_synonym->GetType());
  current_table.AddTargetSynonymValues(main_synonym, entity_list_test);
  ProcessGroup(& current_table, group);
  if (current_table.GetResults()[0].empty()) {
    boolean_result = false;
  }
}

void QueryEvaluator::PreprocessNonBooleanGroup(Group* group, QueryEvaluatorTable* table) {
  Synonym* first_target_synonym = group->GetTargetSynonyms()[0];
  DesignEntity de = first_target_synonym->GetType();

  table->AddTargetSynonymValues(first_target_synonym, db_manager->GetDesignEntities(de));
  ProcessGroup(table, group);
}

Synonym* QueryEvaluator::GetMainSynonymFromClause(Clause* clause) {
  if (typeid(* clause) == typeid(Pattern)) return clause->first_synonym;
  if (clause->left_is_synonym) {
    return clause->first_synonym;
  } else if (clause->right_is_synonym) {
    return clause->second_synonym;
  } else {
    return nullptr;
  }
}
