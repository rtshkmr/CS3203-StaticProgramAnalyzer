#include <model/Statement.h>
#include "ClauseCommandExecutor.h"

ClauseCommandExecutor::ClauseCommandExecutor(QueryEvaluatorTable *table, IntermediateTable *intermediate_table) :
group_table(table), table(intermediate_table) {}

/**
 * The 2 synonyms in the such that clause are already in the group_table. This only restricts rows in the group table
 * and does not add new rows.
 * @param clause The current clause being evaluated.
 */
void ClauseCommandExecutor::SuchThatTwoSynonym(Clause *clause) {
  auto *such_that_clause = dynamic_cast<SuchThat *>(clause);
  int delete_count = 0;

  std::vector<Entity *> first_stmt_list = group_table->GetColumn(such_that_clause->first_synonym);
  std::vector<Entity *> second_stmt_list = group_table->GetColumn(such_that_clause->second_synonym);
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();

  for (int i = 0; i < first_stmt_list.size(); i++) {
    Entity* first_entity = first_stmt_list[i];
    Entity* second_entity = second_stmt_list[i];

    bool relationship_holds = false;
    for (auto result: intermediate_table) {
      if (std::get<0>(result) == first_entity && std::get<1>(result) == second_entity) {
        relationship_holds = true;
        break;
      }
    }
    if (!relationship_holds) {
      group_table->DeleteRow(i - delete_count);
      delete_count++;
    }
  }
}

/**
 * The 2 synonyms in the pattern clause are already in the group_table. This only restricts rows in the group table
 * and does not add new rows. We only need to check the assign synonym as well as the expression.
 * Assume the intermediate table gives a vector of Entity*.
 * @param clause The current clause being evaluated.
 */
void ClauseCommandExecutor::PatternTwoSynonym(Clause *clause) {
  auto *pattern_clause = dynamic_cast<Pattern *>(clause);
  Synonym *first_clause = pattern_clause->first_synonym;
  Synonym *second_clause = pattern_clause->second_synonym;

  std::vector<Entity *> assign_stmt_list = group_table->GetColumn(first_clause);
  std::vector<Entity *> variable_list = group_table->GetColumn(second_clause);
  std::vector<Entity *> intermediate_table = table->GetRelationships();

  unsigned long number_of_iterations = assign_stmt_list.size();
  int delete_count = 0;

  // For each row in the group table
  for (int i = 0; i < assign_stmt_list.size(); i++) {
    AssignEntity *assign_entity_in_table = dynamic_cast<AssignEntity *>(assign_stmt_list[i]);
    bool has_relationship = false;

    for (auto assign_entity: intermediate_table) {
      AssignEntity *current_assign_entity = dynamic_cast<AssignEntity *>(assign_entity);
      if (current_assign_entity == assign_entity_in_table && HasExpressionMatch(pattern_clause, current_assign_entity)) {
        has_relationship = true;
      }
    }

    if (!has_relationship) {
      group_table->DeleteRow(i - delete_count);
      delete_count++;
    }
  }
}

void ClauseCommandExecutor::SuchThatTwoSynonymOneInTable(Clause *clause, bool first_syn_in) {
  auto *such_that_clause = dynamic_cast<SuchThat *>(clause);
  Synonym *synonym_in_table = first_syn_in ? such_that_clause->first_synonym : such_that_clause->second_synonym;
  Synonym *new_synonym = first_syn_in ? such_that_clause->second_synonym : such_that_clause->first_synonym;

  group_table->AddColumn(new_synonym); // Add a new column in the group_table first

  std::vector<Entity *> targetSynonymList = group_table->GetColumn(synonym_in_table);
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();
  unsigned long number_of_times_to_traverse = targetSynonymList.size();
  int target_synonym_list_reference = 0;

  for (int i = 0; i < number_of_times_to_traverse; i++) {    // For each synonym in the table
    Entity *curr_entity = targetSynonymList[target_synonym_list_reference];

    bool hasValidRelationship = false;
    int number_of_repeats = 0;

    for (auto iter: intermediate_table) {
      Entity *entity_to_be_compared = first_syn_in ? std::get<0>(iter) : std::get<1>(iter);
      if (curr_entity == entity_to_be_compared) {
        // Add new row for each col in table
        group_table->AddMultipleRowForAllColumn(new_synonym, i, std::get<1>(iter), number_of_repeats);

        hasValidRelationship = true;
        number_of_repeats++;
      }
    }

    if (number_of_repeats > 0) {
      i += number_of_repeats - 1;
      number_of_times_to_traverse += number_of_repeats - 1;
    }

    // If there are no valid relationships, delete currRow from table.
    if (!hasValidRelationship) {
      group_table->DeleteRow(i);
      i--;
      number_of_times_to_traverse--;
    }
    target_synonym_list_reference++;
  }
}

/**
 * Pattern clause contains assign and variable synonym, however, either might not be in the table.
 * Assume the Intermediate table returns a vector of entity, and this entity is always assign entity.
 * @param clause
 * @param first_syn_in
 */
void ClauseCommandExecutor::PatternTwoSynonymOneInTable(Clause *clause, bool first_syn_in) {
  auto *pattern_clause = dynamic_cast<Pattern *>(clause);
  Synonym *synonym_in_table = first_syn_in ? pattern_clause->first_synonym : pattern_clause->second_synonym;
  Synonym *new_synonym = first_syn_in ? pattern_clause->second_synonym : pattern_clause->first_synonym;

  std::vector<Entity *> entity_list_in_table = group_table->GetColumn(synonym_in_table);
  std::vector<Entity *> intermediate_table = table->GetRelationships();
  int group_table_row_pointer = 0;
  unsigned long table_size = entity_list_in_table.size();
  group_table->AddColumn(new_synonym);

  for (int table_index = 0; table_index < table_size; table_index++) {
    Entity *entity_in_table = entity_list_in_table[group_table_row_pointer];
    int repeat_count = 0;

    // Entity could be assign or variable
    for (auto entity : intermediate_table) {
      AssignEntity *assign_entity = dynamic_cast<AssignEntity *>(entity);
      Variable *variable_entity = assign_entity->GetVariable();
      Entity *entity_to_compare = first_syn_in ? entity : dynamic_cast<Entity*>(variable_entity);

      if ((entity_to_compare != entity_in_table) || (!HasExpressionMatch(pattern_clause, assign_entity))) {
        group_table->DeleteRow(table_index);
        table_index--;
        table_size--;
        continue;
      }

      Entity *entity_to_be_added = first_syn_in ? dynamic_cast<Entity *>(variable_entity) : entity;
      group_table->AddMultipleRowForAllColumn(synonym_in_table, table_index, entity_to_be_added, repeat_count);
      repeat_count++;
    }
    group_table_row_pointer++;
  }
}

void ClauseCommandExecutor::SuchThatOneSynonym(Clause *clause, bool first_syn_in) {
  auto *such_that_clause = dynamic_cast<SuchThat *>(clause);
  std::vector<Entity *> entity_list_to_compare = table->GetRelationships();
  Synonym *syn_in_table = first_syn_in ? such_that_clause->first_synonym : such_that_clause->second_synonym;
  std::vector<Entity *> list_of_entity_in_table = group_table->GetColumn(syn_in_table);
  int delete_count = 0;

  for (int i = 0; i < list_of_entity_in_table.size(); i++) {
    Entity *curr_entity_in_table = list_of_entity_in_table[i];

    bool containsRelationship = false;

    for (auto entity_to_compare : entity_list_to_compare) {
      if (entity_to_compare == curr_entity_in_table) {
        containsRelationship = true;
        break;
      }
    }

    if (!containsRelationship) {
      group_table->DeleteRow(i - delete_count);
      delete_count++;
    }
  }
}

void ClauseCommandExecutor::PatternOneSynonym(Clause *clause) {
  auto *pattern_clause = dynamic_cast<Pattern *>(clause);
  std::vector<Entity *> assign_entity_in_table = group_table->GetColumn(pattern_clause->first_synonym);
  std::vector<Entity *> assign_entity_retrieved = table->GetRelationships();
  unsigned long table_size = assign_entity_in_table.size();
  int delete_count = 0;

  for (int i = 0; i < table_size; i++) {
    Entity *current_assign_stmt = assign_entity_in_table[i];
    bool has_relationship = false;
    // Assert that size of assign_entity_list should be 1.
    for (auto assign_entity_to_compare : assign_entity_retrieved) {
      if (assign_entity_to_compare == current_assign_stmt) {
        has_relationship = true;
        break;
      }
    }
    if (has_relationship) {
      continue;
    }

    group_table->DeleteRow(i - delete_count);
    delete_count++;
  }
}

void ClauseCommandExecutor::SuchThatNoSynonym(Clause *clause) {
  auto *such_that_clause = dynamic_cast<SuchThat *>(clause);
  std::string firstParam = such_that_clause->left_hand_side;
  std::string secondParam = such_that_clause->right_hand_side;
//  if (firstParam == "_" && secondParam == "_") {
//    // Query the PKB for the existence of this relationship
//    return QueryPkbForRelationshipExistence(pkb, st.rel_ref);
//  } else if (firstParam == "_") {
//    std::list<std::tuple<DesignEntity, std::string>>
//    result = QueryPKBSuchThat(pkb, st.rel_ref, st.right_hand_side, false);
//    return !result.empty();    // Return true if there is some value being returned.
//  } else if (secondParam == "_") {
//    std::list<std::tuple<DesignEntity, std::string>>
//    result = QueryPKBSuchThat(pkb, st.rel_ref, st.left_hand_side, true);
//    return !result.empty();    // Return true if there is some value being returned.
//  } else {
//    std::list<std::tuple<DesignEntity, std::string>>
//    result = QueryPKBSuchThat(pkb, st.rel_ref, st.left_hand_side, true);
//    for (auto iter: result) {
//      // Check if second synonym in list
//      if (std::get<1>(iter) == st.right_hand_side) {
//        return true;
//      }
//    }
//    return false;
//  }
}


bool ClauseCommandExecutor::HasExpressionMatch(Pattern *pattern, AssignEntity *assign_entity) {
  std::string expression = pattern->right_hand_side;
  AssignmentExpression* assignment_expression = assign_entity->GetAssignmentExpr();
  if (pattern->is_exact) {
    return assignment_expression->CheckExact(expression);
  } else if (pattern->right_hand_side != "_") {
    return assignment_expression->CheckExist(expression);
  } else {
    return true;
  }
}
