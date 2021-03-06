#include "QuerySystemController.h"
#include "component/QueryProcessor/QueryEvaluator/QueryEvaluator.h"
#include "QueryProjector.h"
#include <component/QueryProcessor/QueryExtractor/QueryExtractor.h>
#include <util/Logger.h>
#include <exception/SpaException.h>

constexpr auto L = [](auto msg) {
  LOG
  (spa_logger << Logger::Prettify(msg));
};

std::vector<std::string> QuerySystemController::Evaluate(std::string* query, PKB* pkb) {
  L("[ENTER] Query System Controller EVALUATE");
  auto query_extractor = QueryExtractor(query);
  try {
    query_extractor.ExtractQuery();
  } catch (const SemanticException) {
    bool was_query_boolean = query_extractor.WasQueryBoolean();
    std::vector<std::string> result;
    if (was_query_boolean) result.emplace_back("FALSE");
    return result;
  } catch (const std::runtime_error& error) {
    return {};
  }

  auto* dbm = new DBManager(pkb);
  auto query_evaluator = QueryEvaluator(dbm);

  L("[ENTER] Query Evaluator Evaluate Query");
  UnformattedQueryResult unformatted_results = query_evaluator.EvaluateQuery(query_extractor.GetGroupsList());

  L("[EXIT] Query Evaluator Evaluate Query ");

  QueryProjector query_projector = QueryProjector(query_extractor.GetTargetSynAttrPairs());
  std::vector<std::string> populated_result_list = query_projector.FormatQuery(unformatted_results);
  L("[EXIT] Query System Controller EVALUATE ");

  dbm->Delete();
  delete dbm;
  return populated_result_list;
}

