//
// Created by Hemanshu Gandhi on 11/9/21.
//

#include "QuerySystemController.h"
#include "component/QueryProcessor/QueryEvaluator/QueryEvaluator.h"
#include "QueryProjector.h"
#include <component/QueryProcessor/QueryExtractor/QueryExtractor.h>

std::vector<std::string> QuerySystemController::Evaluate(std::string* query, PKB* pkb) {
  auto query_extractor = QueryExtractor(query);
  try {
    query_extractor.ExtractQuery();
  } catch (const std::runtime_error& error) {
    // cerr << error.what()<< endl;
    std::vector<std::string> t;
    return t;
  }
  auto query_evaluator = QueryEvaluator(query_extractor.GetSynonymsList(),
                                        query_extractor.GetTarget(),
                                        query_extractor.GetGroupsList(),
                                        *pkb);
  std::vector<std::string> result_list = query_evaluator.EvaluateQuery();
  if (result_list.size() > 0) {
    std::vector<std::string> result_list = QueryProjector::FormatQuery(result_list);
    return result_list;
  }
  return result_list;
};

