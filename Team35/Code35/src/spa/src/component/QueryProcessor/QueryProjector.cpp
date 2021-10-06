#include <list>
#include "QueryProjector.h"

QueryProjector::QueryProjector(std::vector<Synonym> target_synonyms_list) : target_synonym_list(target_synonyms_list) {}

std::vector<std::string> QueryProjector::FormatQuery(UnformattedQueryResult unformatted_results) {
  // For each synonym in the synonym_list
    // Get the unique list of results for that column
  // Need to cross product for multiple synonyms.
  std::vector<QueryEvaluatorTable *> table_references = unformatted_results.GetTables();
  std::vector<std::string> results = {};

  if (unformatted_results.GetBooleanResult() == false) {
    return results;
  }

  for (int i = 0; i < target_synonym_list.size(); i++ ) {
    Synonym current_synonym = target_synonym_list[i];
    for (QueryEvaluatorTable* table : table_references) {
      if (table->ContainsColumn(current_synonym.GetName())) {
        // need to change : Need to change this code for BOOLEAN and tuple cases.
        results = table->GetResults();
      }
    }
  }


  results.erase(std::unique(results.begin(), results.end()), results.end());
  std::vector<std::string> result_list(results.begin(), results.end());
  return result_list;
};
