//
// Created by Max Ng on 13/9/21.
//

#include "EvaluatePatternClause.h"

void ProcessPatternClause(const Pattern pattern, QueryEvaluatorTable* table, const PKB pkb) {
  std::string assignSynonymName = pattern.assign_synonym;
  std::string variableValue = pattern.left_hand_side;

  if (pattern.left_is_synonym) {
    // Case for 2 synonyms
    if (table->ContainsColumn(variableValue) && table->ContainsColumn(assignSynonymName)) {
      // Both synonym in table
      EvaluatePatternDoubleSynonym(pattern, table, pkb);
    } else if (table->ContainsColumn(assignSynonymName)) {
      // Table only contains assign synonym
      EvaluatePatternDoubleSynonymFirstPresent(pattern, table, pkb);
    } else if (table->ContainsColumn(variableValue)) {
      // Table only contains variable synonym
      EvaluatePatternDoubleSynonymSecondPresent(pattern, table, pkb);
    } else {
      // Table contains no synonym
      // Technically this is not possible
    }
  } else {
    // Case of 1 synonym (assign)
    // This section to be targeted in issue 92
    if (table->ContainsColumn(assignSynonymName)) {
      EvaluatePatternSingleSynonym(pattern, table, pkb);
    } else {
      // Technically this should never run
    }
  }
}

void EvaluatePatternDoubleSynonym(Pattern p, QueryEvaluatorTable* table, PKB pkb) {
  // Both synonyms are in table
  std::vector<std::string> assign_stmt_list = table->GetColumn(p.assign_synonym);
  std::vector<std::string> variable_list = table->GetColumn(p.left_hand_side);
  int number_of_iterations = assign_stmt_list.size();

  for (int i = 0; i < number_of_iterations; i++) {
    std::string current_assign_stmt = assign_stmt_list[i];
    std::string current_variable_value = variable_list[i];
    VariableName current_variable_name = VariableName(current_variable_value);
    bool has_relationship = false;

    std::vector<AssignEntity> possible_variable_list = QueryPkbPattern(pkb, true, current_assign_stmt);
    // Assert size == 1
    for (int i = 0; i < possible_variable_list.size(); i++) {
      AssignEntity possible_variable = possible_variable_list[i];
      const VariableName* possible_variable_name = possible_variable.getVariable()->getName();
      if (*possible_variable_name == current_variable_name && HasExpressionMatch(p, possible_variable)) {
        has_relationship = true;
      }
    }

    if (!has_relationship) {
      table->DeleteRow(i);
      i--;
      number_of_iterations--;
    }
  }
}

void EvaluatePatternDoubleSynonymFirstPresent(Pattern p, QueryEvaluatorTable* table, PKB pkb) {
  // Both are synonyms but only assign synonym in table
  std::vector<std::string> assign_stmt_list = table->GetColumn(p.assign_synonym);

  for (int i = 0; i < assign_stmt_list.size(); i++) {
    std::string current_assign_stmt = assign_stmt_list[i];

    std::vector<AssignEntity> assign_entity_list = QueryPkbPattern(pkb, true, current_assign_stmt);

    for (int i = 0; i < assign_entity_list.size(); i++) {
      AssignEntity assign_entity = assign_entity_list[i];

      if (!HasExpressionMatch(p, assign_entity)) {
        continue;
      }

      const VariableName* variable_name = assign_entity.getVariable()->getName();
      VariableName vn = *variable_name;
      std::string name = vn.getName();
      table->AddRowForAllColumn(p.left_hand_side, i, name);
    }

  }
}

void EvaluatePatternDoubleSynonymSecondPresent(Pattern p, QueryEvaluatorTable* table, PKB pkb) {
  // Both are synonyms but only assign synonym in table
  std::vector<std::string> variable_list = table->GetColumn(p.left_hand_side);
  int table_size = variable_list.size();

  for (int table_index = 0; table_index < table_size; table_index++) {
    std::string current_variable = variable_list[table_index];

    std::vector<AssignEntity> assign_entity_list = QueryPkbPattern(pkb, false, current_variable);

    if (assign_entity_list.size() == 0) {
      table->DeleteRow(table_index);
      table_index--;
      table_size--;
      continue;
    }

    for (int i = 0; i < assign_entity_list.size(); i++) {
      AssignEntity assign_entity = assign_entity_list[i];

      if (!HasExpressionMatch(p, assign_entity)) {
        continue;
      }

      const VariableName* variable_name = assign_entity.getVariable()->getName();
      VariableName vn = *variable_name;
      std::string name = vn.getName();
      table->AddRowForAllColumn(p.left_hand_side, i, name);
      // Might have to change the table size pointer here.
    }

  }
}

void EvaluatePatternSingleSynonym(Pattern p, QueryEvaluatorTable* table, PKB pkb) {
  // Both are synonyms but only assign synonym in table
  std::vector<std::string> assign_stmt_list = table->GetColumn(p.assign_synonym);
  int table_size = assign_stmt_list.size();

  for (int i = 0; i < table_size; i++) {
    std::string current_assign_stmt = assign_stmt_list[i];

    std::vector<AssignEntity> assign_entity_list = QueryPkbPattern(pkb, true, current_assign_stmt);
    // Assert that size of assign_entity_list should be 1.
    AssignEntity assign_entity = assign_entity_list[0];

    // Get the retrieved variable name
    const VariableName* variable_name = assign_entity.getVariable()->getName();
    VariableName vn = *variable_name;
    std::string name = vn.getName();

    // Check if the variable name matches
    if (name == p.left_hand_side && HasExpressionMatch(p, assign_entity)) {
      continue;
    }

    table->DeleteRow(i);
    i--;
    table_size--;
  }
}

bool HasExpressionMatch(Pattern p, AssignEntity assign_entity) {
  std::string expression = p.right_hand_side;
  AssignmentExpression* assignment_expression = assign_entity.getAssignmentExpr();

  if (p.is_exact) {
    return assignment_expression->CheckExact(expression);
  } else if (p.right_hand_side != "_") {
    return assignment_expression->CheckExist(expression);
  } else {
    return true;
  }
}
