//
// Created by Max Ng on 14/9/21.
//

#include "EvaluateSuchThatClause.h"

void BothSynonymInTable(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table) {
  std::string firstValue = such_that_clause.left_hand_side;
  std::string secondValue = such_that_clause.right_hand_side;

  std::vector<std::string> firstStmtList = table->GetColumn(firstValue);
  std::vector<std::string> secondStmtList = table->GetColumn(secondValue);

  for (int i = 0; i < firstStmtList.size(); i++) {
    std::string firstStmtRef = firstStmtList[i];
    std::string secondStmtRef = secondStmtList[i];
    std::list<std::tuple<DesignEntity, std::string>> output =
            QueryPKBSuchThat(pkb, such_that_clause.rel_ref, firstStmtRef, true);
    bool relationshipHolds = false;
    for (auto iter = output.begin(); iter != output.end(); iter++) {
      if (std::get<1>(*iter) == secondStmtRef) {
        relationshipHolds = true;
        break;
      }
    }
    if (!relationshipHolds) {
      table->DeleteRow(i);
      i--;
    }
  }
}

void FirstSynonymInTableButSecondNot()

/**
 * Adds a new table column header, then adds or deletes the rows in the table by checking if there is a relationship
 * between the values in the table with all valid stmtRef belonging to the Synonym's DesignEntity.
 *
 * @param target_synonym_name The name of the synonym currently in the table
 * @param new_synonym The synonym to be added.
 * @param table The table which contains the current synonym.
 * @param relationship The type of such-that relationship between the 2 synonyms in question.
 * @param givenFirstParam Checks if the given Synonym is the First or Second Param when querying the PKB
 */
void ProcessNewColumn(std::string target_synonym_name, Synonym new_synonym, QueryEvaluatorTable* table
                      , RelRef relationship, bool givenFirstParam, PKB pkb) {

  table->AddColumn(new_synonym.GetName()); // Add a new column in the table first

  std::vector<std::string> targetSynonymList = table->GetColumn(target_synonym_name);
  int numberOfTimesToTraverse = targetSynonymList.size();
  for (int i = 0; i < numberOfTimesToTraverse; i++) {    // For each synonym in the table
    std::string currStmtRef = targetSynonymList[i];
    // Get the list of possible stmtRef for the current stmtRef.
    std::list<std::tuple<DesignEntity, std::string>> possibleStmtRef =
            QueryPKBSuchThat(pkb, relationship, currStmtRef, givenFirstParam);

    bool hasValidRelationship = false;
    for (auto iter = possibleStmtRef.begin(); iter != possibleStmtRef.end(); iter++) {
      DesignEntity currentStatementType = std::get<0>(*iter);
      std::string currentStatementRef = std::get<1>(*iter);
      if (currentStatementType == new_synonym.GetType()) {
        hasValidRelationship = true;
        // Add new row for each col in table
        table->AddRowForAllColumn(new_synonym.GetName(), i, currentStatementRef);
      }
    }

    // If there are no valid relationships, delete currRow from table.
    if (!hasValidRelationship) {
      table->DeleteRow(i);
      i--;
      numberOfTimesToTraverse--;
    }

  }
}

// Only 1 synonym
void ProcessQueryGivenFirstSynonym(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table) {
  //e.g Parent(a1, "_") or Parent(a1, 3)
  std::string secondParameter = such_that_clause.right_hand_side;
  int deleteCount = 0;
  RelRef relation = such_that_clause.rel_ref;

  // For each row of the synonym in the table, we check if it contains any, or the given stmt number.
  std::vector<std::string> listToVet = table->GetColumn(such_that_clause.left_hand_side);
  for (int i = 0; i < listToVet.size(); i++ ) {
    std::string currentStmtRef = listToVet[i];
    std::list<std::tuple<DesignEntity, std::string>> output = QueryPKBSuchThat(pkb, relation, currentStmtRef, true);

    bool containsRelationship = false;

    if (secondParameter == "_") {
      if (!output.empty()) {
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
      table->DeleteRow(i - deleteCount);
      deleteCount++;
    }
  }
}


void ProcessQueryGivenSecondSynonym(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table) {
  //e.g Parent("_", a1) or Parent(3, a1)
  std::string secondParameter = such_that_clause.right_hand_side;
  RelRef query_relation = such_that_clause.rel_ref;

  // For each row of the synonym in the table, we check if it contains any, or the given stmt number.
  std::vector<std::string> listToVet = table->GetColumn(such_that_clause.left_hand_side);
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
      table->DeleteRow(i);
      i--;
    }
  }
}

bool EvaluateNoSynonym(SuchThat st, PKB pkb) {
  std::string firstParam = st.left_hand_side;
  std::string secondParam = st.right_hand_side;
  if (firstParam == "_" && secondParam == "_") {
    // Query the PKB for the existence of this relationship
    // PKB.hasXxxRelRef()
  } else if (firstParam == "_") {
    std::list<std::tuple<DesignEntity, std::string>> result = QueryPKBSuchThat(pkb, st.rel_ref, st.right_hand_side, false);
    return result.size() != 0;    // Return true if there is some value being returned.
  } else if (secondParam == "_") {
    std::list<std::tuple<DesignEntity, std::string>> result = QueryPKBSuchThat(pkb, st.rel_ref, st.left_hand_side, true);
    return result.size() != 0;    // Return true if there is some value being returned.
  } else {
    std::list<std::tuple<DesignEntity, std::string>> result = QueryPKBSuchThat(pkb, st.rel_ref, st.left_hand_side, true);
    for (auto iter = result.begin(); iter != result.end(); iter++) {
      // Check if second synonym in list
      if (std::get<1>(*iter) == st.right_hand_side) {
        return true;
      }
    }
    return false;
  }
}