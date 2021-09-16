//
// Created by Hemanshu Gandhi on 11/9/21.
//

#include "QuerySystemController.h"
#include "component/QueryProcessor/QueryEvaluator/QueryEvaluator.h"
#include "QueryProjector.h"
#include <component/QueryProcessor/QueryExtractor/QueryExtractor.h>
#include <util/Logger.h>

std::string QuerySystemController::Evaluate(std::string* query, PKB* pkb) {
  LOG (spa_logger << "\n\n\n==========================  [ENTER] Query System Controller EVALUATE ======================\n\n\n");
  auto query_extractor = QueryExtractor(query);
  try {
    query_extractor.ExtractQuery();
  } catch (const std::runtime_error& error) {
    return "";
  }
  auto query_evaluator = QueryEvaluator(query_extractor.GetSynonymsList(),
                                        query_extractor.GetTarget(),
                                        query_extractor.GetGroupsList(),
                                        *pkb);

  LOG (spa_logger << "\n\n\n==========================  [ENTER] Query Evaluator Evaluate Query ======================\n\n\n");
  std::vector<std::string> result_list = query_evaluator.EvaluateQuery();
  LOG (spa_logger << "\n\n\n==========================  [EXIT] Query Evaluator Evaluate Query ======================\n\n\n");
  if (!result_list.empty()) {
    std::string result_string = QueryProjector::FormatQuery(result_list);
    LOG (spa_logger << "\n\n\n==========================  [EXIT] Query System Controller EVALUATE ======================\n\n\n");
    return result_string;
  }
  LOG (spa_logger << "\n\n\n==========================  [EXIT] Query System Controller EVALUATE ======================\n\n\n");
  return "";
};

