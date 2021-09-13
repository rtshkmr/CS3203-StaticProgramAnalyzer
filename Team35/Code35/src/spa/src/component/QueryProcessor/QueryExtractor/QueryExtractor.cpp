#include "QueryExtractor.h"
#include "QueryParser.h"
#include "QueryTokenizer.h"

void QueryExtractor::ExtractQuery() {
  // tokenize, parse, validation logic. Call helper classes to achieve the aforementioned.
  auto tokenizer = QueryTokenizer();
  tokenizer.SetQueryString(&query);
  QueryParser parser = QueryParser(groups, synonyms, target, tokenizer);
  parser.parse();
}