#include "QueryExtractor.h"
#include "QueryParser.h"
#include "QueryTokenizer.h"

void QueryExtractor::extractQuery() {
  // tokenize, parse, validation logic. Call helper classes to achieve the aforementioned.
  auto tokenizer = QueryTokenizer();
  tokenizer.setQueryString(&query);
  QueryParser parser = QueryParser(groups, synonyms, target, tokenizer);
  parser.parse();
}