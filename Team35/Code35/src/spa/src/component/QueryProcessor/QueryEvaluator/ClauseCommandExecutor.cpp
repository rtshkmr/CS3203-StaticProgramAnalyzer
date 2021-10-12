#include <model/Statement.h>
#include "ClauseCommandExecutor.h"

ClauseCommandExecutor::ClauseCommandExecutor(QueryEvaluatorTable *table, IntermediateTable *intermediate_table) :
group_table(table), table(intermediate_table) {}

void ClauseCommandExecutor::SuchThatTwoSynonym(Clause *clause) {
  auto *such_that_clause = dynamic_cast<SuchThat *>(clause);
  int delete_count = 0;

  std::vector<Entity *> first_stmt_list = group_table->GetColumn(&such_that_clause->first_synonym);
  std::vector<Entity *> second_stmt_list = group_table->GetColumn(&such_that_clause->second_synonym);
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

void ClauseCommandExecutor::PatternTwoSynonym(Clause *clause) {
  auto *pattern_clause = dynamic_cast<Pattern *>(clause);
  Synonym *first_clause = &pattern_clause->first_synonym;
  Synonym *second_clause = &pattern_clause->second_synonym;

  std::vector<Entity *> assign_stmt_list = group_table->GetColumn(first_clause);
  std::vector<Entity *> variable_list = group_table->GetColumn(second_clause);
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();

  unsigned long number_of_iterations = assign_stmt_list.size();
  int delete_count = 0;

  for (int i = 0; i < assign_stmt_list.size(); i++) {
    AssignEntity *current_assign_entity = dynamic_cast<AssignEntity *>(assign_stmt_list[i]);
    Entity *current_variable_entity = variable_list[i];
    bool has_relationship = false;

    for (auto row: intermediate_table) {
      Entity* possible_variable = std::get<1>(row);
      if (current_variable_entity == possible_variable && HasExpressionMatch(pattern_clause, current_assign_entity)) {
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
  Synonym *synonym_in_table = first_syn_in ? &such_that_clause->first_synonym : &such_that_clause->second_synonym;
  Synonym *new_synonym = first_syn_in ? &such_that_clause->second_synonym : &such_that_clause->first_synonym;

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

void ClauseCommandExecutor::PatternTwoSynonymOneInTable(Clause *clause, bool first_syn_in) {
  auto *pattern_clause = dynamic_cast<Pattern *>(clause);
  Synonym *synonym_in_table = first_syn_in ? &pattern_clause->first_synonym : &pattern_clause->second_synonym;
  Synonym *new_synonym = first_syn_in ? &pattern_clause->second_synonym : &pattern_clause->first_synonym;

  std::vector<Entity *> assign_stmt_list = group_table->GetColumn(synonym_in_table);
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();
  int assign_statement_pointer = 0;
  unsigned long table_size = assign_stmt_list.size();
  group_table->AddColumn(new_synonym);

  for (int table_index = 0; table_index < table_size; table_index++) {
    Entity *entity_in_table = assign_stmt_list[assign_statement_pointer];
    int repeat_count = 0;

    for (auto iter : intermediate_table) {
      Entity *assign_entity = std::get<0>(iter);
      Entity *variable_entity = std::get<1>(iter);
      Entity *entity_to_be_compared = first_syn_in ? assign_entity : variable_entity;
      AssignEntity *current_assign_entity = dynamic_cast<AssignEntity *>(assign_entity);

      if ((entity_to_be_compared != entity_in_table) || (!HasExpressionMatch(pattern_clause, current_assign_entity))) {
        group_table->DeleteRow(table_index);
        table_index--;
        table_size--;
        continue;
      }

      Entity *entity_to_be_added = first_syn_in ? variable_entity : assign_entity;
      group_table->AddMultipleRowForAllColumn(synonym_in_table, table_index, entity_to_be_added, repeat_count);
      repeat_count++;
    }
    assign_statement_pointer++;
  }
}

void ClauseCommandExecutor::SuchThatOneSynonym(Clause *clause, bool first_syn_in) {
  auto *such_that_clause = dynamic_cast<SuchThat *>(clause);
  std::vector<Entity *> entity_list_to_compare = table->GetRelationships();
  Synonym *syn_in_table = first_syn_in ? &such_that_clause->first_synonym : &such_that_clause->second_synonym;
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
  std::vector<Entity *> assign_entity_in_table = group_table->GetColumn(&pattern_clause->first_synonym);
  std::vector<Entity *> assign_entity_retrieved = table->GetRelationships();
  unsigned long table_size = assign_entity_in_table.size();
  int assign_reference = 0;

  for (int i = 0; i < table_size; i++) {
    Entity *current_assign_stmt = assign_entity_in_table[assign_reference];

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

    group_table->DeleteRow(i);
    i--;
    table_size--;
  }
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
