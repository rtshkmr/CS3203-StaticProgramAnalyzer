#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups, PKB pkb)
        : synonymlist{syn_list}, targetSynonym{target}, groupList{groups}, pkb{pkb} {}

std::vector<std::string> QueryEvaluator::EvaluateQuery() {

  QueryEvaluatorTable targetSynonymTable(targetSynonym.GetName());
  PKB database = pkb;
  std::unordered_map<DesignEntity, std::list<std::string>> map_of_synonym_values;

  // Query all the design entities and add them to an unordered_map.
  for (auto iter = synonymlist.begin(); iter != synonymlist.end(); iter++) {
    DesignEntity synonym_design_entity = iter->GetType();
    std::list<std::string> list_of_synonym_values = database.GetDesignEntity(synonym_design_entity);
    map_of_synonym_values[synonym_design_entity] = list_of_synonym_values;
    if (iter->GetName() == targetSynonym.GetName()) {
      targetSynonymTable.AddTargetSynonym(list_of_synonym_values);
    }
  }

  // For each group, evaluate the group with target synonym first if applicable
  for (auto iter = groupList.begin(); iter != groupList.end(); iter++) {
    if (iter->ContainsTargetSynonym()) {
      std::vector<Clause*> clauseList = iter->GetClauses();
      for (auto iter = clauseList.begin(); iter != clauseList.end(); iter++) {
        Clause* currentClause = *iter;
        if (typeid(*currentClause) == typeid(SuchThat)) {
          // Evaluate such that clause here
          targetSynonymTable = evaluateSuchThat(currentClause, targetSynonymTable);
        } else {
          // Evaluate pattern clause here
        }
      }
    } else {
      // Evaluate boolean type clauses here
    }
  }

  // return the final results
  return targetSynonymTable.GetResults();
};

QueryEvaluatorTable QueryEvaluator::evaluateSuchThat(Clause* clause, QueryEvaluatorTable table) {
  SuchThat* st = dynamic_cast<SuchThat*>(clause);
  RelRef relationship = st->rel_ref;
  table = processDoubleStmtRef(*st, table, relationship);
  return table;
};

QueryEvaluatorTable QueryEvaluator::processDoubleStmtRef(SuchThat such_that_clause, QueryEvaluatorTable table, RelRef query_relation) {
  std::string firstValue = such_that_clause.left_hand_side;
  std::string secondValue = such_that_clause.right_hand_side;
  // All possible cases:
  // Both are synonym
  if (such_that_clause.left_is_synonym && such_that_clause.right_is_synonym) {
    // Both are in the target table
    if (table.ContainsColumn(firstValue) && table.ContainsColumn(secondValue)) {
      table = BothSynonymInTable(pkb, such_that_clause, query_relation, table);

      // First synonym in table, the other is not.
    } else if (table.ContainsColumn(firstValue)) {
      // e.g Parent(a1, a2) where a2 is not in the table
      Synonym newSynonym;
      for (auto iter = synonymlist.begin(); iter != synonymlist.end(); iter++) {
        if (iter->GetName() == secondValue) {
          newSynonym = *iter;
        }
      }
      table = ProcessNewColumn(firstValue, newSynonym, table, such_that_clause.rel_ref, true);

      // Second synonym in table, the other is not.
    } else if (table.ContainsColumn(secondValue)) {
      Synonym newSynonym;
      for (auto iter = synonymlist.begin(); iter != synonymlist.end(); iter++) {
        if (iter->GetName() == firstValue) {
          newSynonym = *iter;
        }
      }
      ProcessNewColumn(secondValue, newSynonym, table, such_that_clause.rel_ref, false);
    } else {
      // None in the target table
      // Nothing should execute here, if the code comes here, maybe throw an error.
    }

  } else if (!such_that_clause.left_is_synonym && !such_that_clause.right_is_synonym) {
    // Boolean case where both are not synonyms. Only supported in 1.3
  } else {
    // Only 1 synonym
    if (such_that_clause.left_is_synonym) {
      //e.g Parent(a1, "_")
      // For each row of the synonym in the table, we check if it contains any, or the given stmt number.
      table = ProcessQueryGivenFirstSynonym(pkb, such_that_clause, query_relation, table);
    } else if (such_that_clause.right_is_synonym) {
      table = ProcessQueryGivenSecondSynonym(pkb, such_that_clause, query_relation, table);
    } else {
      // Assert that no code should ever run here. Throw an error if needed.
    }
  }
  // None is a synonym
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
QueryEvaluatorTable QueryEvaluator::ProcessNewColumn(std::string target_synonym_name, Synonym new_synonym, QueryEvaluatorTable table
                                      , RelRef relationship, bool givenFirstParam) {

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

