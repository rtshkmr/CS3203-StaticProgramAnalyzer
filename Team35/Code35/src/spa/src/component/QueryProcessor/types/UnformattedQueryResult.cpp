#include "UnformattedQueryResult.h"

UnformattedQueryResult::UnformattedQueryResult(bool boolean_result) : passed_all_boolean_group(boolean_result) {}

void UnformattedQueryResult::SetBooleanResult(bool result) {
  passed_all_boolean_group = result;
}

bool UnformattedQueryResult::GetBooleanResult() {
  return passed_all_boolean_group;
}

void UnformattedQueryResult::AddTable(QueryEvaluatorTable *table) {
  table_list.push_back(table);
}

std::vector<QueryEvaluatorTable *> UnformattedQueryResult::GetTables() {
  return table_list;
}