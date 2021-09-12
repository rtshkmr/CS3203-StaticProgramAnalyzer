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

/**
 * Adds a new table column header, then adds or deletes the rows in the table by checking if there is a relationship
 * between the values in the table with all valid stmtRef belonging to the Synonym's DesignEntity.
 *
 * @param target_synonym_name The name of the synonym currently in the table
 * @param new_synonym The synonym to be added.
 * @param table The table which contains the current synonym.
 * @param relationship The type of such-that relationship between the 2 synonyms in question.
 * @param givenFirstParam Checks if the given Synonym is the First or Second Param when querying the PKB
 * @return IfEntity or WhileEntity.
 */
QueryEvaluatorTable ProcessNewColumn(std::string target_synonym_name, Synonym new_synonym, QueryEvaluatorTable table
                                                     , RelRef relationship, bool givenFirstParam, PKB pkb) {

  table.AddColumn(new_synonym.GetName()); // Add a new column in the table first

  std::vector<std::string> targetSynonymList = table.GetColumn(target_synonym_name);
  int numberOfTimesToTraverse = targetSynonymList.size();
  for (int i = 0; i < numberOfTimesToTraverse; i++) {    // For each synonym in the table
    std::string currStmtRef = targetSynonymList[i];
    // Get the list of possible stmtRef for the current stmtRef.
    std::list<std::tuple<DesignEntity, std::string>> possibleStmtRef =
            queryPKBSuchThat(pkb, relationship, currStmtRef, givenFirstParam);

    bool hasValidRelationship = false;
    for (auto iter = possibleStmtRef.begin(); iter != possibleStmtRef.end(); iter++) {
      DesignEntity currentStatementType = std::get<0>(*iter);
      std::string currentStatementRef = std::get<1>(*iter);
      if (currentStatementType == new_synonym.GetType()) {
        hasValidRelationship = true;
        // Add new row for each col in table
        table.AddRowForAllColumn(new_synonym.GetName(), i, currentStatementRef);
      }
    }

    // If there are no valid relationships, delete currRow from table.
    if (!hasValidRelationship) {
      table.DeleteRow(i);
      i--;
      numberOfTimesToTraverse--;
    }

  }
  return table;
}