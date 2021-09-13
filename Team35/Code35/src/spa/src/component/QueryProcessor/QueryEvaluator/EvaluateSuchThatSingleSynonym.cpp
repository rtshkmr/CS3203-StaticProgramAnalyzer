//
// Created by Max Ng on 11/9/21.
//

#include "EvaluateSuchThatSingleSynonym.h"
#include "component/QueryProcessor/types/QueryEvaluatorTable.h"
#include "component/QueryProcessor/types/Types.h"
#include "QueryPKB.h"

// Only 1 synonym
QueryEvaluatorTable ProcessQueryGivenFirstSynonym(PKB pkb, SuchThat such_that_clause, RelRef relation, QueryEvaluatorTable table) {
  //e.g Parent(a1, "_") or Parent(a1, 3)
  std::string secondParameter = such_that_clause.right_hand_side;
  int deleteCount = 0;
  // For each row of the synonym in the table, we check if it contains any, or the given stmt number.
  std::vector<std::string> listToVet = table.GetColumn(such_that_clause.left_hand_side);
  for (int i = 0; i < listToVet.size(); i++ ) {
    std::string currentStmtRef = listToVet[i];
    std::list<std::tuple<DesignEntity, std::string>> output = QueryPKBSuchThat(pkb, relation, currentStmtRef, true);

    bool containsRelationship = false;

    if (secondParameter == "_") {
      if (output.size() != 0) {
        containsRelationship = true;
      }
    } else {
      for (auto resultIter = output.begin(); resultIter != output.end(); resultIter++ ) {
        if (std::get<1>(*resultIter) == secondParameter) {
          containsRelationship = true;
          break;
        }
      }
    }

    if (!containsRelationship) {
      table.DeleteRow(i - deleteCount);
      deleteCount++;
    }
  }

  return table;
}


QueryEvaluatorTable ProcessQueryGivenSecondSynonym(PKB pkb, SuchThat such_that_clause, RelRef query_relation, QueryEvaluatorTable table) {
  //e.g Parent("_", a1) or Parent(3, a1)
  std::string secondParameter = such_that_clause.right_hand_side;

  // For each row of the synonym in the table, we check if it contains any, or the given stmt number.
  std::vector<std::string> listToVet = table.GetColumn(such_that_clause.left_hand_side);
  for (int i = 0; i < listToVet.size(); i++ ) {
    std::string currentStmtRef = listToVet[i];
    std::list<std::tuple<DesignEntity, std::string>> output = QueryPKBSuchThat(pkb, query_relation, currentStmtRef, true);

    bool containsRelationship = false;

    if (secondParameter == "_") {
      if (output.size() != 0) {
        containsRelationship = true;
      }
    } else {
      for (auto resultIter = output.begin(); resultIter != output.end(); resultIter++ ) {
        if (std::get<1>(*resultIter) == secondParameter) {
          containsRelationship = true;
          break;
        }
      }
    }

    if (!containsRelationship) {
      table.DeleteRow(i);
      i--;
    }
  }
  return table;
}
