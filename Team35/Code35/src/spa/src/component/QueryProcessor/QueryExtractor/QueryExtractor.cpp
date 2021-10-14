#include "QueryExtractor.h"
#include "QueryGrouper.h"
#include "QueryParser.h"
#include "QueryTokenizer.h"

/**
 * Creates QueryTokenizer object and QueryParser object. Calls Parse method of QueryParser.
 */
void QueryExtractor::ExtractQuery() {
  auto tokenizer = QueryTokenizer();
  tokenizer.SetQueryString(& query);
  std::vector<Clause*> clauses;
  QueryParser parser = QueryParser(clauses, synonyms, target_synonyms, target_synonyms_map, tokenizer);
  parser.Parse();
  QueryExtractor::PopulateSynAdjacencyList(& map_of_syn_to_clause_indices, & clauses);
  QueryGrouper::GroupClauses(& clauses, & groups, & target_synonyms, &target_synonyms_map,
                             & map_of_syn_to_clause_indices);
}

void QueryExtractor::PopulateSynAdjacencyList(std::unordered_map<std::string, std::vector<int>>* map_of_syn_to_cl_indices,
                                              std::vector<Clause*>* clauses) {
  for (int i = 0; i < clauses->size(); i++) {
    std::vector<std::string> syns = clauses->at(i)->GetAllSynonymNamesOfClause();
    for (std::string s : syns) {
      if (map_of_syn_to_cl_indices->find(s) != map_of_syn_to_cl_indices->end()) {
        map_of_syn_to_cl_indices->at(s).push_back(i);
      } else {
        std::vector<int> v { i };
        map_of_syn_to_cl_indices->insert(std::make_pair(s, v));
      }
    }
  }
}