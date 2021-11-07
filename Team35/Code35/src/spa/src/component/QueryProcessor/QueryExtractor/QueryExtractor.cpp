#include "QueryExtractor.h"
#include "QueryGrouper.h"
#include "QueryParser.h"
/**
 * QueryExtractor is a Facade class for the frontend of Query Subsystem that calls subcomponents for
 * parsing of input, grouping of clauses and optimizations.
 */

void QueryExtractor::ExtractQuery() {
  // note: optimizations are enabled for production; toggling flag to false disables optimizations for testing purposes.
  QueryExtractor::ExtractQuery(true);
}

void QueryExtractor::ExtractQuery(bool should_optimize) {
  auto tokenizer = QueryTokenizer();
  tokenizer.SetQueryString(& query);
  std::vector<Clause*> clauses;
  QueryParser parser = QueryParser(clauses, synonyms, was_query_boolean,
                                   target_syn_attrs, target_synonyms_map, tokenizer);
  parser.Parse();
  if (!should_optimize) {
    QueryGrouper::BasicGroupClauses(& clauses, & groups, &target_synonyms_map);
  } else {
    auto optimizer = QueryOptimizer(clauses, groups, target_syn_attrs, target_synonyms_map);
    optimizer.Optimize();
  }
}

std::vector<std::pair<Synonym*, Attribute>> QueryExtractor::GetTargetSynAttrPairs() {
  for (int i = 0; i < target_syn_attrs.size(); i++) {
    if (target_syn_attrs[i].second == Attribute::kInvalid) {
      target_syn_attrs[i].second = GetDefaultAttribute(target_syn_attrs[i].first->GetType());
    }
  }
  return target_syn_attrs;
}
