#include "TestWrapper.h"
#include <component/QueryProcessor/QueryExtractor/QueryExtractor.h>
#include "component/SourceProcessor/SourceProcessor.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string file_name) {
    sp::SourceProcessor source_processor;
    sp::SourceProcessor::ProcessSourceFile(file_name);
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
// call your evaluator to evaluate the query here
  // ...code to extract query...
  auto query_extractor = new QueryExtractor(query);
  query_extractor->extractQuery();
  // ...code to evaluate query...
  // e.g. evaluateQuery(query_extractor->getSynonymList(), query_extractor->getTarget(), query_extractor->getGroups());

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
}
