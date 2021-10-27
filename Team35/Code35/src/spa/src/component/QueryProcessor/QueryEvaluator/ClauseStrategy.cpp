//
// Created by Max Ng on 13/10/21.
//

#include "ClauseStrategy.h"

ClauseContext::ClauseContext(QueryEvaluatorTable *table) : group_table(table){}

/**
 * Determines the type of the clause and calls the corresponding strategy to create the commands.
 * @param clause Current clause being evaluated.
 * @return DBQueryCommand and ClauseCommand.
 */
std::tuple<DBQueryCommand *, ClauseCommand *> ClauseContext::ProcessClause(Clause *clause) {
  ClauseStrategy *strategy;
  if (typeid(*clause) == typeid(SuchThat)) {
    strategy = new SuchThatStrategy();
  } else {
    strategy = new PatternStrategy();
  }
  return strategy->DetermineClauseCommand(clause, group_table);
}

std::tuple<DBQueryCommand *, ClauseCommand *>
ClauseStrategy::DetermineDoubleSynonymCommands(QueryEvaluatorTable *table, Synonym *first_synonym,
                                               Synonym *second_synonym, Clause *clause) {
  DBQueryCommand *query_command = nullptr;
  if (typeid(*clause) == typeid(SuchThat)) {
    query_command = new QuerySuchThatTwoSynonymCommand(clause);
  } else {
    query_command = new QueryPatternTwoSynonymCommand(clause);
  }
  ClauseCommand *clause_command = nullptr;
  if (table->ContainsColumn(first_synonym) && table->ContainsColumn(second_synonym)) {
    clause_command = new DoubleSynonymBothPresentCommand();
  } else if (table->ContainsColumn(first_synonym)) {
    clause_command = new DoubleSynonymSinglePresentCommand(true);
  } else if (table->ContainsColumn(second_synonym)) {
    clause_command = new DoubleSynonymSinglePresentCommand(false);
  } else {
    // Should not be the case unless cross product
  }
  return std::make_tuple(query_command, clause_command);
}

std::tuple<DBQueryCommand *, ClauseCommand *>
ClauseStrategy::DetermineSingleSynonymCommand(Clause *clause, bool synonym_is_first_param) {
  DBQueryCommand *query_command = nullptr;
  if (typeid(*clause) == typeid(SuchThat)) {
    query_command = new QuerySuchThatOneSynonymCommand(clause);
  } else {
    query_command = new QueryPatternOneSynonymCommand(clause);
  }
  ClauseCommand *clause_command = new SingleSynonymPresentCommand(synonym_is_first_param);
  return std::make_tuple(query_command, clause_command);
}

/**
 * Determines the commands to be used during the pipeline of the clause evaluation.
 * @param clause The clause to be evaluated.
 * @param table
 * @return
 */
std::tuple<DBQueryCommand *, ClauseCommand *>
SuchThatStrategy::DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) {
  auto* such_that_clause = dynamic_cast<SuchThat*>(clause);
  DBQueryCommand *query_command = nullptr;
  ClauseCommand *clause_command = nullptr;

  if (such_that_clause->left_is_synonym && such_that_clause->right_is_synonym) {
    // Both are synonyms
    return DetermineDoubleSynonymCommands(table, such_that_clause->first_synonym, such_that_clause->second_synonym, clause);
  } else if (such_that_clause->left_is_synonym) {
    return DetermineSingleSynonymCommand(clause, true);
  } else if (such_that_clause->right_is_synonym) {
    return DetermineSingleSynonymCommand(clause, false);
  } else {
    query_command = new QuerySuchThatNoSynonymCommand(clause);
    return std::make_tuple(query_command, clause_command);
  }
}

std::tuple<DBQueryCommand *, ClauseCommand *>
PatternStrategy::DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) {
  auto* pattern_clause = dynamic_cast<Pattern*>(clause);
  Synonym *assign_synonym = pattern_clause->first_synonym;
  DBQueryCommand *query_command = nullptr;
  ClauseCommand *clause_command = nullptr;

  if (pattern_clause->left_is_synonym) {
    Synonym *variable_synonym = pattern_clause->second_synonym;
    return DetermineDoubleSynonymCommands(table, assign_synonym, variable_synonym, clause);
  } else {
    if (table->ContainsColumn(assign_synonym)) {
      return DetermineSingleSynonymCommand(clause, true);
    } else {
      // Assign is not in the table yet, cross product
    }
  }
  return std::make_tuple(query_command, clause_command);
}
