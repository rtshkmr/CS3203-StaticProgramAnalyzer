#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group> groups, PKB pkb)
        : synonymlist{syn_list}, targetSynonym{target}, groupList{groups}, pkb{pkb}, booleanResult{true},
        map_of_synonym_values{} {}

std::vector<std::string> QueryEvaluator::EvaluateQuery() {

  QueryEvaluatorTable targetSynonymTable(targetSynonym.GetName());

  PopulateSynonymValues(&targetSynonymTable);
  EvaluateAllGroups(&targetSynonymTable);

  // Check the boolean conditions and return the final results
  if (!booleanResult) {
    std::vector<std::string> emptyList = {};
    return emptyList;
  } else {
    return targetSynonymTable.GetResults();
  }
}

void QueryEvaluator::PopulateSynonymValues(QueryEvaluatorTable* table) {
  // Query all the design entities and add them to an unordered_map.
  for (auto iter = synonymlist.begin(); iter != synonymlist.end(); iter++) {
    DesignEntity synonym_design_entity = iter->GetType();
    std::string synonym_name = iter->GetName();
    std::list<std::string> list_of_synonym_values = pkb.GetDesignEntity(synonym_design_entity);
    map_of_synonym_values[synonym_name] = list_of_synonym_values;
    if (iter->GetName() == targetSynonym.GetName()) {
      table->AddTargetSynonym(list_of_synonym_values);
    }
  }
}

void QueryEvaluator::EvaluateAllGroups(QueryEvaluatorTable* table) {
  // For each group, evaluate the group with target synonym first if applicable
  for (auto iter = groupList.begin(); iter != groupList.end(); iter++) {
    std::vector<Clause*> clauseList = iter->GetClauses();

    // This Group contains the target synonym
    if (iter->ContainsTargetSynonym()) {
      ProcessNonBooleanGroup(clauseList, table);

      // This Group contains no target synonym and is treated as a boolean type group
    } else {
      // Each boolean group has their own table.
      ProcessBooleanGroup(clauseList);
    }
  }
}

void QueryEvaluator::ProcessNonBooleanGroup(std::vector<Clause*> clauseList, QueryEvaluatorTable* table) {

  for (auto iter = clauseList.begin(); iter != clauseList.end(); iter++) {
    Clause* currentClause = *iter;

    if (typeid(*currentClause) == typeid(SuchThat)) {
      // Evaluate such that clause here
      SuchThat* st = dynamic_cast<SuchThat*>(currentClause);
      EvaluateSuchThatClause(*st, table);
    } else {
      // Evaluate pattern clause here
      Pattern* p = dynamic_cast<Pattern*>(currentClause);
      ProcessPatternClause(*p, table, pkb);
    }
  }
}

void QueryEvaluator::EvaluateSuchThatClause(SuchThat such_that_clause, QueryEvaluatorTable* table) {
  std::string firstValue = such_that_clause.left_hand_side;
  std::string secondValue = such_that_clause.right_hand_side;
  // All possible cases:
  // Both are synonym
  if (such_that_clause.left_is_synonym && such_that_clause.right_is_synonym) {
    // Both are in the target table
    if (table->ContainsColumn(firstValue) && table->ContainsColumn(secondValue)) {
      BothSynonymInTable(pkb, such_that_clause, table);

      // First synonym in table, the other is not.
    } else if (table->ContainsColumn(firstValue)) {
      // e.g Parent(a1, a2) where a2 is not in the table
      Synonym newSynonym;
      for (auto iter = synonymlist.begin(); iter != synonymlist.end(); iter++) {
        if (iter->GetName() == secondValue) {
          newSynonym = *iter;
        }
      }
      ProcessNewColumn(firstValue, newSynonym, table,
                               such_that_clause.rel_ref, true, pkb);

      // Second synonym in table, the other is not.
    } else if (table->ContainsColumn(secondValue)) {
      Synonym newSynonym;
      for (auto iter = synonymlist.begin(); iter != synonymlist.end(); iter++) {
        if (iter->GetName() == firstValue) {
          newSynonym = *iter;
        }
      }
      ProcessNewColumn(secondValue, newSynonym, table,
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
      ProcessQueryGivenFirstSynonym(pkb, such_that_clause, table);
    } else {
      ProcessQueryGivenSecondSynonym(pkb, such_that_clause, table);
    }
  }
}

void QueryEvaluator::ProcessBooleanGroup(std::vector<Clause*> clauseList) {
  Clause* firstClause = clauseList[0];
  std::string synonym_name = "";
  if (typeid(*firstClause) == typeid(SuchThat)) {
    SuchThat* st = dynamic_cast<SuchThat*>(firstClause);
    if (st->left_is_synonym) {
      synonym_name = st->left_hand_side;
    } else if (st->right_is_synonym) {
      synonym_name = st->right_hand_side;
    } else {
      // If no synonym, no need for a table. Also, should be in a group of size 1.
      bool result = EvaluateNoSynonym(*st, pkb);
      if (result == false) {
        booleanResult = false;        // Call to the class variable directly
      }
    }
  } else if (typeid(*firstClause) == typeid(Pattern)) {
    Pattern* pattern = dynamic_cast<Pattern*>(firstClause);
    synonym_name = pattern->assign_synonym;
  } else {
    // No code should run here for iter 1 since there is only such that and pattern clause.
  }

  if (synonym_name != "") {
    QueryEvaluatorTable currTable(synonym_name);
    currTable.AddTargetSynonym(map_of_synonym_values[synonym_name]);

    for (auto iter = clauseList.begin(); iter != clauseList.end(); iter++) {
      Clause* currentClause = *iter;
      if (typeid(*currentClause) == typeid(SuchThat)) {
        // Evaluate such that clause here
        SuchThat* currentSuchThat = dynamic_cast<SuchThat*>(currentClause);
        EvaluateSuchThatClause(*currentSuchThat, &currTable);
      } else {
        // Evaluate pattern clause here
        // Technically not possible here
      }
    }

    if (currTable.GetSize() == 0) {
      booleanResult = false;
    }
  }
}
