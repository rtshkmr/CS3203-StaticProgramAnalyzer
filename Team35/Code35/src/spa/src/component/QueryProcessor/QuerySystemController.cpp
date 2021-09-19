//
// Created by Hemanshu Gandhi on 11/9/21.
//

#include "QuerySystemController.h"
#include "component/QueryProcessor/QueryEvaluator/QueryEvaluator.h"
#include "QueryProjector.h"
#include <component/QueryProcessor/QueryExtractor/QueryExtractor.h>
#include <util/Logger.h>

constexpr auto L = [](auto msg) {
  LOG
  (spa_logger << Logger::Prettify(msg));
};

std::string QuerySystemController::Evaluate(std::string* query, PKB* pkb) {
  L("[ENTER] Query System Controller EVALUATE");
  auto query_extractor = QueryExtractor(query);
  try {
    query_extractor.ExtractQuery();
  } catch (const std::runtime_error& error) {
    return "";
  }
  auto query_evaluator = QueryEvaluator(query_extractor.GetSynonymsList(),
                                        query_extractor.GetTarget(),
                                        query_extractor.GetGroupsList(),
                                        * pkb);

  L("[ENTER] Query Evaluator Evaluate Query");
  std::vector<std::string> result_list = query_evaluator.EvaluateQuery();
  L("[EXIT] Query Evaluator Evaluate Query ");
  if (!result_list.empty()) {
    std::string result_string = QueryProjector::FormatQuery(result_list);
    L("[EXIT] Query System Controller EVALUATE ");
    return result_string;
  }
  L("[EXIT] Query System Controller EVALUATE ");
  return "";
};

