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

  std::vector<Entity *> first_entity_stmt_list = group_table->GetColumn(first_clause);
  std::vector<Entity *> variable_list = group_table->GetColumn(second_clause);
  std::vector<Entity *> intermediate_table = table->GetRelationships();

  int delete_count = 0;

  // For each row in the group table
  for (int i = 0; i < first_entity_stmt_list.size(); i++) {
    Entity *current_entity = first_entity_stmt_list[i];
    bool has_relationship;
    if (typeid(*current_entity) == typeid(AssignEntity)) {
      has_relationship = HasAssignPatternRelationship(current_entity, variable_list[i], pattern_clause);
    } else {
      has_relationship = HasWhileOrIfPatternRelationship(current_entity, variable_list[i]);
    }

    if (!has_relationship) {
      group_table->DeleteRow(i - delete_count);
      delete_count++;
    }
  }
}

bool ClauseCommandExecutor::HasAssignPatternRelationship(Entity *assign_entity, Entity *variable_entity, Pattern *pattern) {
  std::vector<Entity *> intermediate_table = table->GetRelationships();
  bool has_relationship = false;

  for (auto current_entity: intermediate_table) {
    AssignEntity *current_assign_entity = dynamic_cast<AssignEntity *>(current_entity);
    Variable *current_variable_entity = current_assign_entity->GetVariable();
    if (current_assign_entity == assign_entity && variable_entity == current_variable_entity &&
        HasExpressionMatch(pattern, current_assign_entity)) {
      has_relationship = true;
      break;
    }
  }
  return has_relationship;
}

bool ClauseCommandExecutor::HasWhileOrIfPatternRelationship(Entity *left_entity, Entity *variable_entity) {
  std::vector<Entity *> intermediate_table = table->GetRelationships();
  bool has_relationship = false;

  for (auto first_entity: intermediate_table) {
    std::vector<Variable*> control_variables;
    if (typeid(*left_entity) == typeid(WhileEntity)) {
      WhileEntity *current_entity = dynamic_cast<WhileEntity *>(first_entity);
      control_variables = current_entity->GetControlVariables();
    } else {
      IfEntity *current_entity = dynamic_cast<IfEntity *>(first_entity);
      control_variables = current_entity->GetControlVariables();
    }

    for (auto current_variable_entity : control_variables) {
      if (left_entity == first_entity && variable_entity == current_variable_entity) {
        has_relationship = true;
      }
    }

    if (has_relationship) break;
  }
  return has_relationship;
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

    //
    for (auto iter: intermediate_table) {
      Entity *entity_to_be_compared = first_syn_in ? std::get<0>(iter) : std::get<1>(iter);
      Entity *entity_to_be_added = first_syn_in ? std::get<1>(iter) : std::get<0>(iter);
      if (curr_entity == entity_to_be_compared) {
        // Add new row for each col in table
        group_table->AddMultipleRowForAllColumn(new_synonym, i, entity_to_be_added, number_of_repeats);

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
  unsigned long table_size = entity_list_in_table.size();
  group_table->AddColumn(new_synonym);
  int group_table_pointer = 0;

  for (int table_index = 0; table_index < table_size; table_index++) {
    Entity *entity_in_table = entity_list_in_table[table_index];
    int repeat_count = 0;
    bool has_relation = false;

    // Entity could be stmt entity or variable
    if (first_syn_in) {
      std::tuple<bool, int> tuple = PatternRowAdditionForVariable(group_table_pointer, new_synonym, entity_in_table, pattern_clause);
      has_relation = std::get<0>(tuple);
      repeat_count = std::get<1>(tuple);
    } else {
      std::tuple<bool, int> tuple = PatternRowAdditionForStmt(group_table_pointer, new_synonym, entity_in_table, pattern_clause);
      has_relation = std::get<0>(tuple);
      repeat_count = std::get<1>(tuple);
    }

    if (repeat_count > 0) group_table_pointer += repeat_count - 1;
    if (!has_relation) {
      group_table->DeleteRow(group_table_pointer);
      group_table_pointer--;
    }
    group_table_pointer++;
  }
}

/**
 * Iterate through the stmt entity in the intermediate table to compare all Variables associated with the stmt, then add
 * it if there is a match. The expression for assign entities must be matched.
 * @param index The index to start adding the new rows.
 * @param synonym_column_to_add The stmt Synonym column to add new values to.
 * @param entity_in_table The Variable entity in the group table to be compared to.
 * @return
 */
std::tuple<bool, int>
ClauseCommandExecutor::PatternRowAdditionForStmt(int index, Synonym *synonym_column_to_add, Entity *entity_in_table, Pattern *pattern) {
  std::vector<Entity *> intermediate_table = table->GetRelationships();
  int repeat_count = 0;
  bool has_relation = false;

  for (auto stmt_entity : intermediate_table) {
    bool stmt_contains_variable = false;
    std::vector<Variable *> variable_to_check = RetrieveVariablesFromStmt(stmt_entity);

    for (auto variable : variable_to_check) {
      // Need to check that the variable and expression matches
      bool match_found = false;
      if (typeid(*stmt_entity) == typeid(AssignEntity)) {
        match_found = variable == entity_in_table && HasExpressionMatch(pattern, dynamic_cast<AssignEntity *>(stmt_entity));
      } else {
        match_found = variable == entity_in_table;
      }
      if (match_found) {
        stmt_contains_variable = true;
        has_relation = true;

        group_table->AddMultipleRowForAllColumn(synonym_column_to_add, index, stmt_entity, repeat_count);
        repeat_count++;
      }
    }
  }

  return std::make_tuple(has_relation, repeat_count);
}

/**
 * Iterate through the stmt entity in the intermediate table to find the entity which matches the entity_in_table, then
 * retrieve the Variables associated with it and add those variables to the table.
 * The entity in the table should be of the stmt entity type (Assign, While, if).
 * @param index The index to start adding the values at.
 * @param synonym_column_to_add The variable synonym column to add to.
 * @param entity_in_table The current stmt entity in the table.
 * @return a tuple indicating if at least a row has been added and the number of times a row has been added.
 */
std::tuple<bool, int>
ClauseCommandExecutor::PatternRowAdditionForVariable(int index, Synonym *synonym_column_to_add, Entity *entity_in_table, Pattern *pattern) {
  std::vector<Entity *> intermediate_table = table->GetRelationships();
  int repeat_count = 0;
  bool has_relation = false;

  for (auto stmt_entity : intermediate_table) {
    bool match_found = false;
    if (typeid(*stmt_entity) == typeid(AssignEntity)) {
      match_found = stmt_entity == entity_in_table && HasExpressionMatch(pattern, dynamic_cast<AssignEntity *>(stmt_entity));
    } else {
      match_found = stmt_entity == entity_in_table;
    }
    if (!match_found) {
      continue;
    }

    std::vector<Variable *> variable_to_add = RetrieveVariablesFromStmt(stmt_entity);

    for (auto variable : variable_to_add) {
      group_table->AddMultipleRowForAllColumn(synonym_column_to_add, index, dynamic_cast<Entity *>(variable), repeat_count);
      repeat_count++;
    }

    has_relation = true;
    break;
  }
  return std::make_tuple(has_relation, repeat_count);
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

/**
 * Check all stmt entity and check if the value provided matches the value of the Variable(s) in the stmt entity.
 * @param clause
 */
void ClauseCommandExecutor::PatternOneSynonym(Clause *clause) {
  auto *pattern_clause = dynamic_cast<Pattern *>(clause);
  std::vector<Entity *> assign_entity_in_table_list = group_table->GetColumn(pattern_clause->first_synonym);
  std::string left_side_value = pattern_clause->left_hand_side;
  unsigned long table_size = assign_entity_in_table_list.size();
  int delete_count = 0;

  for (int i = 0; i < table_size; i++) {
    Entity *current_assign_stmt = assign_entity_in_table_list[i];

    if (!HasPatternValueMatch(current_assign_stmt, left_side_value, pattern_clause)) {
      group_table->DeleteRow(i - delete_count);
      delete_count++;
    }
  }
}

bool ClauseCommandExecutor::HasPatternValueMatch(Entity *stmt_entity_in_table, std::string value, Pattern *pattern) {
  std::vector<Variable *> variable_to_check = RetrieveVariablesFromStmt(stmt_entity_in_table);

  if (typeid(*stmt_entity_in_table) == typeid(AssignEntity)) {
    AssignEntity *assign_entity = dynamic_cast<AssignEntity *>(stmt_entity_in_table);
    std::string variable_value_in_table = const_cast<VariableName*>(variable_to_check[0]->GetName())->getName();
    return (value == "_" || value == variable_value_in_table) && HasExpressionMatch(pattern, assign_entity);
  } else {
    for (auto variable : variable_to_check) {
      std::string variable_value_in_table = const_cast<VariableName*>(variable->GetName())->getName();
      if (value == "_" || variable_value_in_table == value) return true;
    }
    return false;
  }
}

std::vector<Variable *> ClauseCommandExecutor::RetrieveVariablesFromStmt(Entity *stmt_entity) {
  std::vector<Variable *> variable_to_check;
  if (typeid(*stmt_entity) == typeid(AssignEntity)) {
    AssignEntity *assign_entity = dynamic_cast<AssignEntity *>(stmt_entity);
    variable_to_check.push_back(assign_entity->GetVariable());
  } else if (typeid(*stmt_entity) == typeid(IfEntity)) {
    IfEntity *if_entity = dynamic_cast<IfEntity *>(stmt_entity);
    for (auto variable : if_entity->GetControlVariables()) {
      variable_to_check.push_back(variable);
    }
  } else if (typeid(*stmt_entity) == typeid(WhileEntity)) {
    WhileEntity *if_entity = dynamic_cast<WhileEntity *>(stmt_entity);
    for (auto variable : if_entity->GetControlVariables()) {
      variable_to_check.push_back(variable);
    }
  }
  return variable_to_check;
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
