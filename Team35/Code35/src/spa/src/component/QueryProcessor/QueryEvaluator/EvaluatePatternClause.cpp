#include "EvaluatePatternClause.h"

/**
 * For each pattern clause, there are many different types of configurations provided to the pattern clause, and each
 * type will be sent to a relevant method to further process that type of clause.
 * @param pattern The pattern clause
 * @param table The table containing the synonym of the current pattern clause.
 * @param pkb The pkb containing the information about the source program.
 * @param synonym_design_entity_map The unordered map, mapping the name of the synonym to its design entity.
 */
void ProcessPatternClause(const Pattern& pattern, QueryEvaluatorTable* table, const PKB& pkb,
                          std::unordered_map<std::string, DesignEntity> synonym_design_entity_map) {
  std::string assign_synonym_name = pattern.assign_synonym;
  std::string variable_value = pattern.left_hand_side;

  if (pattern.left_is_synonym) {
    // Case for 2 synonyms
    if (table->ContainsColumn(variable_value) && table->ContainsColumn(assign_synonym_name)) {
      // Both synonym in table
      EvaluatePatternDoubleSynonym(pattern, table, pkb);
    } else if (table->ContainsColumn(assign_synonym_name)) {
      // Table only contains assign synonym
      EvaluatePatternDoubleSynonymFirstPresent(pattern, table, pkb);
    } else if (table->ContainsColumn(variable_value)) {
      // Table only contains variable synonym
      EvaluatePatternDoubleSynonymSecondPresent(pattern, table, pkb, synonym_design_entity_map);
    } else {
      // Table contains no synonym
      // Technically this is not possible
    }
  } else {
    // Case of 1 synonym (assign)
    // This section to be targeted in issue 92
    if (table->ContainsColumn(assign_synonym_name)) {
      EvaluatePatternSingleSynonym(pattern, table, pkb);
    } else {
      // Technically this should never run
    }
  }
}

void EvaluatePatternDoubleSynonym(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb) {
  // Both synonyms are in table
  std::vector<std::string> assign_stmt_list = table->GetColumn(p.assign_synonym);
  std::vector<std::string> variable_list = table->GetColumn(p.left_hand_side);
  unsigned long number_of_iterations = assign_stmt_list.size();
  int table_index = 0;

  for (int current_table_index = 0; current_table_index < number_of_iterations; current_table_index++) {
    std::string current_assign_stmt = assign_stmt_list[current_table_index];
    std::string current_variable_value = variable_list[current_table_index];
    VariableName current_variable_name = VariableName(current_variable_value);
    bool has_relationship = false;

    std::vector<AssignEntity> possible_variable_list = QueryPkbPattern(pkb, true, current_assign_stmt);
    // Assert size == 1
    for (auto possible_variable : possible_variable_list) {
      const VariableName* possible_variable_name = possible_variable.GetVariable()->GetName();
      if (*possible_variable_name == current_variable_name && HasExpressionMatch(p, possible_variable)) {
        has_relationship = true;
      }
    }

    if (!has_relationship) {
      table->DeleteRow(table_index);
      number_of_iterations--;
      table_index--;
    }
    table_index++;
  }
}

void EvaluatePatternDoubleSynonymFirstPresent(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb) {
  // Both are synonyms but only assign synonym in table
  std::vector<std::string> assign_stmt_list = table->GetColumn(p.assign_synonym);
  int assign_statement_pointer = 0;
  unsigned long table_size = assign_stmt_list.size();
  table->AddColumn(p.left_hand_side);

  for (int table_index = 0; table_index < table_size; table_index++) {
    std::string current_assign_stmt = assign_stmt_list[assign_statement_pointer];
    int repeat_count = 0;

    std::vector<AssignEntity> assign_entity_list = QueryPkbPattern(pkb, true, current_assign_stmt);

    for (int i = 0; i < assign_entity_list.size(); i++) {
      AssignEntity assign_entity = assign_entity_list[i];

      if (!HasExpressionMatch(p, assign_entity)) {
        table->DeleteRow(table_index);
        table_index--;
        table_size--;
        continue;
      }

      const VariableName* variable_name = assign_entity.GetVariable()->GetName();
      VariableName vn = *variable_name;
      std::string name = vn.getName();
      table->AddMultipleRowForAllColumn(p.left_hand_side, i, name, repeat_count);
      repeat_count++;
    }
    assign_statement_pointer++;
  }
}

void EvaluatePatternDoubleSynonymSecondPresent(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb,
                                               std::unordered_map<std::string, DesignEntity> synonym_design_entity_map) {
  // Both are synonyms but only variable synonym in table
  std::vector<std::string> variable_list = table->GetColumn(p.left_hand_side);
  std::string stmt_synonym = table->GetStatementSynonym(synonym_design_entity_map);
  table->AddColumn(p.assign_synonym);
  int variable_list_reference = 0;
  unsigned long table_size = variable_list.size();

  for (int table_index = 0; table_index < table_size; table_index++) {
    std::string current_variable = variable_list[variable_list_reference];
    std::vector<AssignEntity> assign_entity_list = QueryPkbPattern(pkb, false, current_variable);

    bool has_variable = false;
    int repeat_count = 0;

    for (auto assign_entity : assign_entity_list) {
      const VariableName* variable_name = assign_entity.GetVariable()->GetName();
      VariableName vn = *variable_name;
      std::string name = vn.getName();
      std::string statement_number = std::to_string(assign_entity.GetStatementNumber()->GetNum());

      // If the LHS or RHS of the pattern does not match, do not add it to the table.
      if (name != current_variable || !HasExpressionMatch(p, assign_entity)) {
        continue;
      }

      has_variable = true;
      table->AddMultipleRowForAllColumn(p.assign_synonym, table_index, statement_number, repeat_count);
      repeat_count++;
    }

    // If there are no variables matching at all, delete this current row since it does not hold true.
    if (!has_variable) {
      table->DeleteRow(table_index);
      table_index--;
      table_size--;
    } else {
      table_index += repeat_count -1;
      table_size += repeat_count -1;
    }
    variable_list_reference++;
  }
}

void EvaluatePatternSingleSynonym(const Pattern& p, QueryEvaluatorTable* table, const PKB& pkb) {
  // Both are synonyms but only assign synonym in table
  std::vector<std::string> assign_stmt_list = table->GetColumn(p.assign_synonym);
  unsigned long table_size = assign_stmt_list.size();
  int assign_reference = 0;

  for (int i = 0; i < table_size; i++) {
    std::string current_assign_stmt = assign_stmt_list[assign_reference];

    std::vector<AssignEntity> assign_entity_list = QueryPkbPattern(pkb, true, current_assign_stmt);
    // Assert that size of assign_entity_list should be 1.
    AssignEntity assign_entity = assign_entity_list[0];

    // Get the retrieved variable name
    const VariableName* variable_name = assign_entity.GetVariable()->GetName();
    VariableName vn = *variable_name;
    std::string name = vn.getName();

    assign_reference++;
    // Check if the variable name matches
    if ((name == p.left_hand_side || p.left_hand_side == "_") && HasExpressionMatch(p, assign_entity)) {
      continue;
    }

    table->DeleteRow(i);
    i--;
    table_size--;
  }
}

bool HasExpressionMatch(const Pattern& p, AssignEntity assign_entity) {
  std::string expression = p.right_hand_side;
  AssignmentExpression* assignment_expression = assign_entity.GetAssignmentExpr();

  if (p.is_exact) {
    return assignment_expression->CheckExact(expression);
  } else if (p.right_hand_side != "_") {
    return assignment_expression->CheckExist(expression);
  } else {
    return true;
  }
}
