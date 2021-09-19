//
// Created by Max Ng on 14/9/21.
//

#include "EvaluateSuchThatClause.h"

void EvaluateSuchThatClause(SuchThat such_that_clause, QueryEvaluatorTable* table, PKB pkb,
                            std::list<Synonym> synonym_list) {
  std::string firstValue = such_that_clause.left_hand_side;
  std::string secondValue = such_that_clause.right_hand_side;

  if (such_that_clause.left_is_synonym && such_that_clause.right_is_synonym) {
    // Both are in the target table
    if (table->ContainsColumn(firstValue) && table->ContainsColumn(secondValue)) {
      BothSynonymInTable(pkb, such_that_clause, table);

    } else if (table->ContainsColumn(firstValue)) {
      // e.g Parent(a1, a2) where a2 is not in the table
      Synonym newSynonym = GetSynonymFromName(secondValue, synonym_list);
      ProcessNewColumn(firstValue, newSynonym, table,
                       such_that_clause.rel_ref, true, pkb);

      // Second synonym in table, the other is not.
    } else if (table->ContainsColumn(secondValue)) {
      Synonym newSynonym = GetSynonymFromName(firstValue, synonym_list);
      ProcessNewColumn(secondValue, newSynonym, table,
                       such_that_clause.rel_ref, false, pkb);
    } else {
      // Neither synonym in the target table
      // Nothing should execute here, if the code comes here, maybe throw an error.
    }

  } else if (such_that_clause.left_is_synonym) {
    ProcessQueryGivenFirstSynonym(pkb, such_that_clause, table);
  } else {
    ProcessQueryGivenSecondSynonym(pkb, such_that_clause, table);
  }
}

void BothSynonymInTable(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table) {
    std::string first_value = such_that_clause.left_hand_side;
    std::string second_value = such_that_clause.right_hand_side;
    int delete_count = 0;

    std::vector<std::string> first_stmt_list = table->GetColumn(first_value);
    std::vector<std::string> second_stmt_list = table->GetColumn(second_value);

    for (int i = 0; i < first_stmt_list.size(); i++) {
        std::string firstStmtRef = first_stmt_list[i];
        std::string secondStmtRef = second_stmt_list[i];
        std::list<std::tuple<DesignEntity, std::string>> output =
            QueryPKBSuchThat(pkb, such_that_clause.rel_ref, firstStmtRef, true);
        bool relationship_holds = false;
        for (auto iter = output.begin(); iter != output.end(); iter++) {
            if (std::get<1>(*iter) == secondStmtRef) {
                relationship_holds = true;
                break;
            }
        }
        if (!relationship_holds) {
            table->DeleteRow(i - delete_count);
            delete_count++;
        }
    }
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
 * @param pkb The provided PKB object
 */
void ProcessNewColumn(std::string target_synonym_name, Synonym new_synonym, QueryEvaluatorTable* table
                      , RelRef relationship, bool givenFirstParam, PKB pkb) {

    table->AddColumn(new_synonym.GetName()); // Add a new column in the table first

    std::vector<std::string> targetSynonymList = table->GetColumn(target_synonym_name);
    int number_of_times_to_traverse = targetSynonymList.size();
    int target_synonym_list_reference = 0;

    for (int i = 0; i < number_of_times_to_traverse; i++) {    // For each synonym in the table
        std::string currStmtRef = targetSynonymList[target_synonym_list_reference];
        // Get the list of possible stmtRef for the current stmtRef.
        std::list<std::tuple<DesignEntity, std::string>> possibleStmtRef =
            QueryPKBSuchThat(pkb, relationship, currStmtRef, givenFirstParam);

        bool hasValidRelationship = false;
        int number_of_repeats = 0;

        for (auto iter = possibleStmtRef.begin(); iter != possibleStmtRef.end(); iter++) {
            DesignEntity currentStatementType = std::get<0>(*iter);
            std::string currentStatementRef = std::get<1>(*iter);
            if (SynonymTypeMatches(currentStatementType, new_synonym.GetType())) {
                // Add new row for each col in table
                table->AddMultipleRowForAllColumn(new_synonym.GetName(), i, currentStatementRef, number_of_repeats);

                hasValidRelationship = true;
                number_of_repeats++;
            }
        }

        if (number_of_repeats > 0) {
            i += number_of_repeats - 1;
            number_of_times_to_traverse += number_of_repeats -1;
        }

        // If there are no valid relationships, delete currRow from table.
        if (!hasValidRelationship) {
            table->DeleteRow(i);
            i--;
            number_of_times_to_traverse--;
        }
        target_synonym_list_reference++;
    }
}

bool SynonymTypeMatches(DesignEntity current_synonym, DesignEntity synonym_type_to_match) {
  if (synonym_type_to_match == DesignEntity::kStmt) {
    switch (current_synonym) {
      case DesignEntity::kStmt:return true;
      case DesignEntity::kRead:return true;
      case DesignEntity::kPrint:return true;
      case DesignEntity::kCall:return true;
      case DesignEntity::kWhile:return true;
      case DesignEntity::kIf:return true;
      case DesignEntity::kAssign:return true;
      case DesignEntity::kVariable:return false;
      case DesignEntity::kConstant:return false;
      case DesignEntity::kProcedure:return false;
      case DesignEntity::kInvalid:return false;
      default:return false;
    }
  } else {
    return current_synonym == synonym_type_to_match;
  }

}

// Only 1 synonym
void ProcessQueryGivenFirstSynonym(PKB pkb, SuchThat such_that_clause, QueryEvaluatorTable* table) {
    //e.g Parent(a1, "_") or Parent(a1, 3)
    std::string second_parameter = such_that_clause.right_hand_side;
    int deleteCount = 0;
    RelRef relation = such_that_clause.rel_ref;

    // For each row of the synonym in the table, we check if it contains any, or the given stmt number.
    std::vector<std::string> listToVet = table->GetColumn(such_that_clause.left_hand_side);
    for (int i = 0; i < listToVet.size(); i++ ) {
        std::string currentStmtRef = listToVet[i];
        std::list<std::tuple<DesignEntity, std::string>> output = QueryPKBSuchThat(pkb, relation, currentStmtRef, true);

        bool containsRelationship = false;

        if (second_parameter == "_") {
            if (!output.empty()) {
                containsRelationship = true;
            }
        } else {
            for (auto resultIter = output.begin(); resultIter != output.end(); resultIter++ ) {
              std::string possible_value = std::get<1>(*resultIter);
                if (possible_value == second_parameter) {
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
    std::string first_parameter = such_that_clause.left_hand_side;
    std::string second_parameter = such_that_clause.right_hand_side;
    RelRef query_relation = such_that_clause.rel_ref;
    int deleteCount = 0;

    // For each row of the synonym in the table, we check if it contains any, or the given stmt number.
    std::vector<std::string> listToVet = table->GetColumn(second_parameter);
    for (int i = 0; i < listToVet.size(); i++ ) {
        std::string currentStmtRef = listToVet[i];
        std::list<std::tuple<DesignEntity, std::string>> output = QueryPKBSuchThat(pkb, query_relation, currentStmtRef, false);

        bool containsRelationship = false;

        if (first_parameter == "_") {
            if (output.size() != 0) {
                containsRelationship = true;
            }
        } else {
            for (auto resultIter = output.begin(); resultIter != output.end(); resultIter++ ) {
                if (std::get<1>(*resultIter) == first_parameter) {
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

bool EvaluateNoSynonym(SuchThat st, PKB pkb) {
    std::string firstParam = st.left_hand_side;
    std::string secondParam = st.right_hand_side;
    if (firstParam == "_" && secondParam == "_") {
        // Query the PKB for the existence of this relationship
        return QueryPkbForRelationshipExistence(pkb, st.rel_ref);
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

Synonym GetSynonymFromName(std::string synonym_name, std::list<Synonym> synonym_list) {
  Synonym new_synonym;
  for (auto iter = synonym_list.begin(); iter != synonym_list.end(); iter++) {
    if (iter->GetName() == synonym_name) {
      new_synonym = *iter;
      return new_synonym;
    }
  }
}
