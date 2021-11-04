#include <model/Statement.h>
#include "ClauseCommandExecutor.h"

ClauseCommandExecutor::ClauseCommandExecutor(QueryEvaluatorTable *table, IntermediateTable *intermediate_table) :
group_table(table), table(intermediate_table) {}

/**
 * The 2 synonyms in the clause are already in the group_table. This only restricts rows in the group table
 * and does not add new rows. The check is then dispatched according to the type of the clause to check if there
 * is a valid case based on the conditions in the clause and decides if the row should be deleted.
 * @param clause
 */
void ClauseCommandExecutor::DoubleSynonymIntersection(Clause *clause) {
  int delete_count = 0;
  int table_size = group_table->GetRowSize();

  for (int i = 0; i < table_size; i++) {
    bool relationship_holds = DoubleSynonymIntersectionCheck(clause, i - delete_count);

    if (!relationship_holds) {
      group_table->DeleteRow(i - delete_count);
      delete_count++;
    }
  }
}

bool ClauseCommandExecutor::DoubleSynonymIntersectionCheck(Clause *clause, int index) {
  if (typeid(*clause) == typeid(SuchThat)) {
    return SuchThatDoubleSynonymIntersection(dynamic_cast<SuchThat *>(clause), index);
  } else if (typeid(*clause) == typeid(Pattern)) {
    return PatternDoubleSynonymIntersection(dynamic_cast<Pattern *>(clause), index);
  } else {
    return WithDoubleSynonymIntersection(dynamic_cast<With *>(clause), index);
  }
}

bool ClauseCommandExecutor::SuchThatDoubleSynonymIntersection(SuchThat *clause, int index) {
  Synonym *first_syn = clause->first_synonym;
  Synonym *second_syn = clause->second_synonym;
  Entity* first_entity = group_table->GetColumn(first_syn)[index];
  Entity* second_entity = group_table->GetColumn(second_syn)[index];
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();
  for (auto result: intermediate_table) {
    if (std::get<0>(result) == first_entity && std::get<1>(result) == second_entity) {
      return true;
    }
  }
  return false;
}

bool ClauseCommandExecutor::PatternDoubleSynonymIntersection(Pattern *clause, int index) {
  Synonym *first_synonym = clause->first_synonym;
  Synonym *second_synonym = clause->second_synonym;
  std::vector<Entity *> first_entity_stmt_list = group_table->GetColumn(first_synonym);
  std::vector<Entity *> variable_list = group_table->GetColumn(second_synonym);
  Entity *current_entity = first_entity_stmt_list[index];

  if (typeid(*current_entity) == typeid(AssignEntity)) {
    return HasAssignPatternRelationship(current_entity, variable_list[index], clause);
  } else {
    return HasWhileOrIfPatternRelationship(current_entity, variable_list[index]);
  }
}

bool ClauseCommandExecutor::WithDoubleSynonymIntersection(With *with_clause, int index) {
  std::vector<Entity *> first_stmt_list = group_table->GetColumn(with_clause->first_synonym);
  std::vector<Entity *> second_stmt_list = group_table->GetColumn(with_clause->second_synonym);
  Entity* first_entity = first_stmt_list[index];
  Entity* second_entity = second_stmt_list[index];
  std::string first_entity_value = RetrieveEntityAttribute(first_entity, with_clause->left_attribute);
  std::string second_entity_value = RetrieveEntityAttribute(second_entity, with_clause->right_attribute);
  return first_entity_value == second_entity_value;
}


bool ClauseCommandExecutor::HasAssignPatternRelationship(Entity *assign_entity, Entity *variable_entity, Pattern *pattern) {
  std::vector<Entity *> intermediate_table = table->GetRelationships();
  bool has_relationship = false;

  for (auto current_entity: intermediate_table) {
    auto *current_assign_entity = dynamic_cast<AssignEntity *>(current_entity);
    Variable *current_variable_entity = current_assign_entity->GetVariableObj();
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
      auto *current_entity = dynamic_cast<WhileEntity *>(first_entity);
      control_variables = current_entity->GetControlVariables();
    } else {
      auto *current_entity = dynamic_cast<IfEntity *>(first_entity);
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

std::string ClauseCommandExecutor::RetrieveEntityAttribute(Entity *entity, Attribute attribute) {
  switch (attribute) {
  case (Attribute::kStmtNumber):
    return std::to_string(const_cast<StatementNumber*>(dynamic_cast<Statement *>(entity)->GetStatementNumber())->GetNum());
  case Attribute::kProcName :
    if (typeid(*entity) == typeid(Procedure)) {
      return const_cast<ProcedureName*>(dynamic_cast<Procedure *>(entity)->GetName())->getName();
    } else {
      return const_cast<ProcedureName*>(dynamic_cast<CallEntity *>(entity)->GetProcedure()->GetName())->getName();
    }
  case Attribute::kValue:
    return std::to_string(const_cast<ConstantValue*>(dynamic_cast<Constant *>(entity)->GetValue())->Get());
  case Attribute::kVarName:
    if (typeid(*entity) == typeid(Variable)) {
      return dynamic_cast<Variable*>(entity)->GetNameInString();
    } else if (typeid(*entity) == typeid(ReadEntity)) {
      return dynamic_cast<ReadEntity *>(entity)->GetVariable()->GetNameInString();
    } else {
      return dynamic_cast<PrintEntity *>(entity)->GetVariable()->GetNameInString();
    }
  default:
    return "";
  }
}

/**
 * Takes in a clause with two synonyms, one in the table and the other not, then adds or deletes rows according to the
 * type of clause and its respective evaluation.
 * @param clause The current clause being evaluated, irregardless of association with group type.
 * @param first_syn_in True if the first synonym of the clause is in the table, false if the second synonym is in
 * the table.
 */
void ClauseCommandExecutor::DoubleSynonymExpansion(Clause *clause, bool first_syn_in) {
  int group_table_pointer = 0;
  int table_size = group_table->GetRowSize();
  Synonym* new_synonym = first_syn_in ? clause->second_synonym : clause->first_synonym;
  group_table->AddColumn(new_synonym);
  for (int i = 0; i < table_size; i++) {
    // Send to dispatcher to reallocate the work that returns repeat_count and has_relation.
    std::tuple<bool, int> tuple = DoubleSynonymExpansionCheck(clause, first_syn_in, group_table_pointer);
    bool has_relation = std::get<0>(tuple);
    int repeat_count = std::get<1>(tuple);
    if (repeat_count > 0) group_table_pointer += repeat_count - 1;
    if (!has_relation) {
      group_table->DeleteRow(group_table_pointer);
      group_table_pointer--;
    }
    group_table_pointer++;
  }
}

std::tuple<bool, int> ClauseCommandExecutor::DoubleSynonymExpansionCheck(Clause *clause, bool first_syn_in, int index) {
  if (typeid(*clause) == typeid(SuchThat)) {
    return SuchThatRowAddition(dynamic_cast<SuchThat*>(clause), first_syn_in, index);
  } else if (typeid(*clause) == typeid(Pattern)) {
    return PatternRowAddition(dynamic_cast<Pattern*>(clause), first_syn_in, index);
  } else {
    return WithRowAddition(dynamic_cast<With *>(clause), first_syn_in, index);
  }
}

std::tuple<bool, int> ClauseCommandExecutor::SuchThatRowAddition(SuchThat *clause, bool first_syn_in, int index) {
  Synonym *synonym_in_table = first_syn_in ? clause->first_synonym : clause->second_synonym;
  Synonym *new_synonym = first_syn_in ? clause->second_synonym : clause->first_synonym;
  std::vector<Entity *> target_synonym_list = group_table->GetColumn(synonym_in_table);
  Entity *curr_entity = target_synonym_list[index];
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();

  int number_of_repeats = 0;
  bool has_valid_relationship = false;
  for (auto iter: intermediate_table) {
    Entity *entity_to_be_compared = first_syn_in ? std::get<0>(iter) : std::get<1>(iter);
    Entity *entity_to_be_added = first_syn_in ? std::get<1>(iter) : std::get<0>(iter);
    // Add new row for each col in table
    if (curr_entity == entity_to_be_compared) {
      group_table->AddMultipleRowForAllColumn(new_synonym, index, entity_to_be_added, number_of_repeats);
      has_valid_relationship = true;
      number_of_repeats++;
    }
  }
  return std::make_tuple(has_valid_relationship, number_of_repeats);
}

std::tuple<bool, int> ClauseCommandExecutor::PatternRowAddition(Pattern *pattern_clause, bool first_syn_in, int index) {
  Synonym *synonym_in_table = first_syn_in ? pattern_clause->first_synonym : pattern_clause->second_synonym;
  Synonym *new_synonym = first_syn_in ? pattern_clause->second_synonym : pattern_clause->first_synonym;
  std::vector<Entity *> entity_list_in_table = group_table->GetColumn(synonym_in_table);
  Entity *entity_in_table = entity_list_in_table[index];
  if (first_syn_in) {
    return PatternRowAdditionForVariable(index, new_synonym, entity_in_table, pattern_clause);
  } else {
    return PatternRowAdditionForStmt(index, new_synonym, entity_in_table, pattern_clause);
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
ClauseCommandExecutor::PatternRowAdditionForStmt(int index, Synonym *synonym_column_to_add, Entity *variable_in_table,
                                                 Pattern *pattern) {
  std::vector<Entity *> intermediate_table = table->GetRelationships();
  int repeat_count = 0;
  bool has_relation = false;
  for (auto stmt_entity : intermediate_table) {
    std::vector<Variable *> variable_to_check = RetrieveVariablesFromStmt(stmt_entity);
    // Need to check that the variable and expression matches, then add the stmt
    for (auto variable : variable_to_check) {
      bool match_found;
      if (typeid(*stmt_entity) == typeid(AssignEntity)) {
        match_found = variable == variable_in_table &&
                HasExpressionMatch(pattern, dynamic_cast<AssignEntity *>(stmt_entity));
      } else {
        match_found = variable == variable_in_table;
      }
      if (match_found) {
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
    bool match_found;
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

std::tuple<bool, int>
ClauseCommandExecutor::WithRowAddition(With *with_clause, bool first_syn_in, int index) {
  Synonym *synonym_in_table = first_syn_in ? with_clause->first_synonym : with_clause->second_synonym;
  Synonym *new_synonym = first_syn_in ? with_clause->second_synonym : with_clause->first_synonym;
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();
  std::vector<Entity *> entity_list_in_table = group_table->GetColumn(synonym_in_table);
  int number_of_repeats = 0;
  bool has_valid_relationship = false;

  for (auto iter : intermediate_table) {
    Entity *entity_to_be_compared = first_syn_in ? std::get<0>(iter) : std::get<1>(iter);
    Entity *entity_to_be_added = first_syn_in ? std::get<1>(iter) : std::get<0>(iter);
    if (entity_list_in_table[index] == entity_to_be_compared) {
      group_table->AddMultipleRowForAllColumn(new_synonym, index, entity_to_be_added, number_of_repeats);
      number_of_repeats++;
      has_valid_relationship = true;
    }
  }
  return std::make_tuple(has_valid_relationship, number_of_repeats);
}

void ClauseCommandExecutor::SingleSynonymIntersection(Clause *clause, bool first_syn_in) {
  int delete_count = 0;
  int table_size = group_table->GetRowSize();

  for (int i = 0; i < table_size; i++) {
    bool relationship_exists = SingleSynonymIntersectionCheck(clause, first_syn_in, i - delete_count);

    if (!relationship_exists) {
      group_table->DeleteRow(i - delete_count);
      delete_count++;
    }
  }
}

bool ClauseCommandExecutor::SingleSynonymIntersectionCheck(Clause *clause, bool first_syn_in, int index) {
  if (typeid(*clause) == typeid(SuchThat)) {
    return SuchThatSingleSynonymIntersection(dynamic_cast<SuchThat *>(clause), first_syn_in, index);
  } else if (typeid(*clause) == typeid(Pattern)) {
    return PatternSingleSynonymIntersection(dynamic_cast<Pattern *>(clause), index);
  } else {
    return WithSingleSynonymIntersection(dynamic_cast<With *>(clause), first_syn_in, index);
  }
}

bool ClauseCommandExecutor::SuchThatSingleSynonymIntersection(SuchThat *clause, bool first_syn_in, int index) {
  std::vector<Entity *> entity_list_to_compare = table->GetRelationships();
  Synonym *syn_in_table = first_syn_in ? clause->first_synonym : clause->second_synonym;
  Entity *curr_entity_in_table = group_table->GetColumn(syn_in_table)[index];

  for (auto entity_to_compare : entity_list_to_compare) {
    if (entity_to_compare == curr_entity_in_table) {
      return true;
    }
  }
  return false;
}

bool ClauseCommandExecutor::PatternSingleSynonymIntersection(Pattern *clause, int index) {
  std::vector<Entity *> assign_entity_in_table_list = group_table->GetColumn(clause->first_synonym);
  std::string left_side_value = clause->left_hand_side;
  Entity *current_assign_stmt = assign_entity_in_table_list[index];
  return HasPatternValueMatch(current_assign_stmt, left_side_value, clause);
}

bool ClauseCommandExecutor::WithSingleSynonymIntersection(With *with_clause, bool first_syn_in, int index) {
  std::vector<Entity *> entity_list_to_compare = table->GetRelationships();
  Synonym *syn_in_table = first_syn_in ? with_clause->first_synonym : with_clause->second_synonym;
  Entity *curr_entity_in_table = group_table->GetColumn(syn_in_table)[index];

  for (auto entity_to_compare : entity_list_to_compare) {
    if (entity_to_compare == curr_entity_in_table) {
      return true;
    }
  }
  return false;
}

bool ClauseCommandExecutor::HasPatternValueMatch(Entity *stmt_entity_in_table, const std::string& value, Pattern *pattern) {
  std::vector<Variable *> variable_to_check = RetrieveVariablesFromStmt(stmt_entity_in_table);

  if (typeid(*stmt_entity_in_table) == typeid(AssignEntity)) {
    auto *assign_entity = dynamic_cast<AssignEntity *>(stmt_entity_in_table);
    std::string variable_value_in_table = const_cast<VariableName*>(variable_to_check[0]->GetVariableName())->GetName();
    return (value == "_" || value == variable_value_in_table) && HasExpressionMatch(pattern, assign_entity);
  } else {
    for (auto variable : variable_to_check) {
      std::string variable_value_in_table = const_cast<VariableName*>(variable->GetVariableName())->GetName();
      if (value == "_" || variable_value_in_table == value) return true;
    }
    return false;
  }
}

std::vector<Variable *> ClauseCommandExecutor::RetrieveVariablesFromStmt(Entity *stmt_entity) {
  std::vector<Variable *> variable_to_check;
  if (typeid(*stmt_entity) == typeid(AssignEntity)) {
    auto *assign_entity = dynamic_cast<AssignEntity *>(stmt_entity);
    variable_to_check.push_back(assign_entity->GetVariableObj());
  } else if (typeid(*stmt_entity) == typeid(IfEntity)) {
    auto *if_entity = dynamic_cast<IfEntity *>(stmt_entity);
    for (auto variable : if_entity->GetControlVariables()) {
      variable_to_check.push_back(variable);
    }
  } else if (typeid(*stmt_entity) == typeid(WhileEntity)) {
    auto *if_entity = dynamic_cast<WhileEntity *>(stmt_entity);
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

void ClauseCommandExecutor::DoubleSynonymCross(Clause *clause) {
  Synonym *first_syn = clause->first_synonym;
  Synonym *second_syn = clause->second_synonym;
  std::vector<std::tuple<Entity *, Entity *>> intermediate_table = table->GetRelationshipsByType();
  std::vector<Entity*> first_entity_list;
  std::vector<Entity*> second_entity_list;
  for (auto tuple : intermediate_table) {
    if (typeid(*clause) == typeid(Pattern*) && typeid(*clause->first_synonym) == typeid(AssignEntity)) {
      if (!HasAssignPatternRelationship(std::get<0>(tuple),
                                        std::get<1>(tuple), dynamic_cast<Pattern*>(clause))) continue;
    }
    first_entity_list.push_back(std::get<0>(tuple));
    second_entity_list.push_back(std::get<1>(tuple));
  }
  std::vector<std::vector<Entity*>> entity_list = {first_entity_list, second_entity_list};
  std::vector<Synonym *> synonym_list = {first_syn, second_syn};
  group_table->CrossProductColumns(synonym_list, entity_list);
}

void ClauseCommandExecutor::SingleSynonymCross(Clause *clause, bool syn_is_first_param) {
  Synonym *new_synonym = syn_is_first_param ? clause->first_synonym : clause->second_synonym;
  std::vector<Entity*> entity_list = table->GetRelationships();
  if (typeid(*clause) == typeid(Pattern*) && typeid(*clause->first_synonym) == typeid(AssignEntity)) {
    entity_list = FilterAssignSingleClause(entity_list, clause, syn_is_first_param);
  }
  std::vector<std::vector<Entity*>> final_entity_list = {entity_list};
  std::vector<Synonym *> synonym_list = {new_synonym};
  group_table->CrossProductColumns(synonym_list, final_entity_list);
}

std::vector<Entity *> ClauseCommandExecutor::FilterAssignSingleClause(const std::vector<Entity *>& unfiltered_entities, Clause *clause, bool is_first) {
  std::vector<Entity *> filtered_entities;

  for (auto entity : unfiltered_entities) {
    auto assign_entity = dynamic_cast<AssignEntity*>(entity);
    Variable* variable_entity = assign_entity->GetVariableObj();
    if (!HasAssignPatternRelationship(entity, variable_entity
                                      , dynamic_cast<Pattern*>(clause))) continue;
    if (is_first) {
      filtered_entities.push_back(entity);
    } else {
      filtered_entities.push_back(variable_entity);
    }
  }
  return filtered_entities;
}
