#include "QueryExtractor.h"
#include "QueryOptimizer.h"
#include "QueryParser.h"
#include "QueryTokenizer.h"

/**
 * Creates QueryTokenizer object and QueryParser object. Calls Parse method of QueryParser.
 */
void QueryExtractor::ExtractQuery() {
  auto tokenizer = QueryTokenizer();
  tokenizer.SetQueryString(& query);
  std::vector<Clause*> clauses;
  QueryParser parser = QueryParser(clauses, groups, synonyms, target_synonyms, tokenizer);
  parser.Parse();
  QueryOptimizer::GroupClauses(& clauses, & groups, & target_synonyms);
}
