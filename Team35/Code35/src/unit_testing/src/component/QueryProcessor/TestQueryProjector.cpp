#include "catch.hpp"
#include <list>
#include <model/Statement.h>
#include "component/QueryProcessor/QueryProjector.h"
#include "../../../utils/EntityUtils.h"

TEST_CASE("3.QueryProjector.1 target synonym") {
  AssignEntity* a1 = entity_utils::GetAssign1();
  a1->SetStatementNumber(new StatementNumber(1));
  AssignEntity* a2 = entity_utils::GetAssign2();
  a2->SetStatementNumber(new StatementNumber(2));
  AssignEntity* a3 = entity_utils::GetAssign3();
  a3->SetStatementNumber(new StatementNumber(3));

  Synonym* target_syn = new Synonym("a", DesignEntity::kAssign);
  QueryEvaluatorTable* table1 = new QueryEvaluatorTable(target_syn);
  std::vector<Entity*> col = std::vector<Entity*>{
    a1, a2, a3
  };
  table1->AddTargetSynonymValues(col);

  UnformattedQueryResult uqr = UnformattedQueryResult(true);
  uqr.AddTable(table1);

  std::vector<Synonym> syn_list = std::vector<Synonym>{*target_syn};
  QueryProjector qp = QueryProjector(syn_list);

  std::vector<std::string> expected_values = std::vector<std::string>{"1", "2", "3"};
  std::vector<std::string> actual_values = qp.FormatQuery(uqr);

  CHECK(expected_values == actual_values);
}
