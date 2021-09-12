#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups, PKB pkb)
        : synonymlist{syn_list}, targetSynonym{target}, groupList{groups}, pkb{pkb}, booleanResult{true} {}

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
      // Each boolean group has their own table.
      std::vector<Clause*> clauseList = iter->GetClauses();
      Clause* firstClause = clauseList[0];
      std::string synonymName = "";
      if (typeid(*firstClause) == typeid(SuchThat)) {
        SuchThat* st = dynamic_cast<SuchThat*>(firstClause);
        if (st->left_is_synonym) {
          synonymName = st->left_hand_side;
        } else if (st->right_is_synonym) {
          synonymName = st->right_hand_side;
        } else {
          // If no synonym, no need for a table. Also, should be in a group of size 1.
          bool result = EvaluateNoSynonym(*st, pkb);
          if (result == false) {
            booleanResult = false;
            break;
          }
          continue;
        }
      } else if (typeid(*firstClause) == typeid(Pattern)) {
        Pattern* pattern = dynamic_cast<Pattern*>(firstClause);
        synonymName = pattern->assign_synonym;
      } else {
        // No code should run here for iter 1 since there is only suchthat and pattern clause.
      }

      if (synonymName != "") {
        QueryEvaluatorTable currTable(synonymName);
        for (auto iter = clauseList.begin(); iter != clauseList.end(); iter++) {
          Clause* currentClause = *iter;
          if (typeid(*currentClause) == typeid(SuchThat)) {
            // Evaluate such that clause here
            SuchThat* currentSuchThat = dynamic_cast<SuchThat*>(currentClause);
            currTable = processBooleanSuchThat(*currentSuchThat);
          } else {
            // Evaluate pattern clause here
          }
        }

        if (currTable.GetSize() == 0) {
          booleanResult = false;
          break;
        }
      }
    }
  }

  // Check the boolean conditions
  if (booleanResult == false) {
    std::vector<std::string> emptyList = {};
    return emptyList;
  }

  // return the final results
  return targetSynonymTable.GetResults();
};

QueryEvaluatorTable QueryEvaluator::evaluateSuchThat(Clause* clause, QueryEvaluatorTable table) {
  SuchThat* st = dynamic_cast<SuchThat*>(clause);
  RelRef relationship = st->rel_ref;
  table = processNonBooleanSuchThat(*st, table, relationship);
  return table;
};

QueryEvaluatorTable QueryEvaluator::processNonBooleanSuchThat(SuchThat such_that_clause, QueryEvaluatorTable table, RelRef query_relation) {
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
      table = ProcessNewColumn(firstValue, newSynonym, table,
                               such_that_clause.rel_ref, true, pkb);

      // Second synonym in table, the other is not.
    } else if (table.ContainsColumn(secondValue)) {
      Synonym newSynonym;
      for (auto iter = synonymlist.begin(); iter != synonymlist.end(); iter++) {
        if (iter->GetName() == firstValue) {
          newSynonym = *iter;
        }
      }
      table = ProcessNewColumn(secondValue, newSynonym, table,
                       such_that_clause.rel_ref, false, pkb);
    } else {
      // Neither synonym in the target table
      // Nothing should execute here, if the code comes here, maybe throw an error.
    }

  } else if (!such_that_clause.left_is_synonym && !such_that_clause.right_is_synonym) {
    // Boolean case where both are not synonyms.
    // This should throw an error since it belongs to a group with target synonym.
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

QueryEvaluatorTable QueryEvaluator::processBooleanSuchThat(SuchThat such_that_clause) {

}