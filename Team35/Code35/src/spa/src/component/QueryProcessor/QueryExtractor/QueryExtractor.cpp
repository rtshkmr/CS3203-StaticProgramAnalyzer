#include "QueryExtractor.h"
#include "QueryParser.h"
#include "QueryTokenizer.h"

/**
 * Creates QueryTokenizer object and QueryParser object. Calls Parse method of QueryParser.
 */
void QueryExtractor::ExtractQuery() {
  auto tokenizer = QueryTokenizer();
  tokenizer.SetQueryString(&query);
  std::vector<Clause*> clauses;
  QueryParser parser = QueryParser(clauses, groups, synonyms, target, tokenizer);
  parser.Parse();
}