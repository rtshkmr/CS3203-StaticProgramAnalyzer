//
// Created by Max Ng on 13/9/21.
//

#include "EvaluatePatternDoubleSynonym.h"

QueryEvaluatorTable EvaluatePatternDoubleSynonym(Pattern p, QueryEvaluatorTable table, PKB pkb) {
  // Both synonyms are in table
  std::vector<std::string> assign_stmt_list = table.GetColumn(p.assign_synonym);
  std::vector<std::string> variable_list = table.GetColumn(p.left_hand_side);

  for (int i = 0; i < assign_stmt_list.size(); i++) {
    std::string current_assign_stmt = assign_stmt_list[i];
    std::string current_variable_value = variable_list[i];
    VariableName current_variable_name = VariableName(current_variable_value);
    bool has_relationship = false;

    std::vector<AssignEntity> possible_variable_list = QueryPkbPattern(pkb, true, current_assign_stmt);
    for (int i = 0; i < possible_variable_list.size(); i++) {
      AssignEntity possible_variable = possible_variable_list[i];
      const VariableName* possible_variable_name = possible_variable.getVariable()->getName();
      if (*possible_variable_name == current_variable_name) {
        has_relationship = true;
      }
    }

    if (!has_relationship) {
      table.DeleteRow(i);
      i--;
    }
  }
}