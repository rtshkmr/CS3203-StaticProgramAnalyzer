//
// Created by Max Ng on 11/9/21.
//
#include "EvaluateSuchThatDoubleSynonyms.h"

QueryEvaluatorTable BothSynonymInTable(PKB pkb, SuchThat such_that_clause, RelRef relation, QueryEvaluatorTable table) {
  std::string firstValue = such_that_clause.left_hand_side;
  std::string secondValue = such_that_clause.right_hand_side;

  if (table.ContainsColumn(firstValue) && table.ContainsColumn(secondValue)) {
    std::vector<std::string> firstStmtList = table.GetColumn(firstValue);
    std::vector<std::string> secondStmtList = table.GetColumn(secondValue);

    for (int i = 0; i < firstStmtList.size(); i++) {
      std::string firstStmtRef = firstStmtList[i];
      std::string secondStmtRef = secondStmtList[i];
      std::list<std::tuple<DesignEntity, std::string>> output =
              queryPKBSuchThat(pkb, such_that_clause.rel_ref, firstStmtRef, true);
      bool relationshipHolds = false;
      for (auto iter = output.begin(); iter != output.end(); iter++) {
        if (std::get<1>(*iter) == secondStmtRef) {
          relationshipHolds = true;
          break;
        }
      }
      if (!relationshipHolds) {
        table.DeleteRow(i);
        i--;
      }
    }
  }

  return table;
}