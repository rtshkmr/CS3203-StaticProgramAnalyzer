//
// Created by Max Ng on 13/10/21.
//

#include "ClauseStrategy.h"
#include <cassert>

ClauseContext::ClauseContext(QueryEvaluatorTable *table) : group_table(table){}

/**
 * Determines the type of the clause and calls the corresponding strategy to create the commands.
 * @param clause Current clause being evaluated.
 * @return PKBQueryCommand and ClauseCommand.
 */
std::tuple<PKBQueryCommand *, ClauseCommand *> ClauseContext::ProcessClause(Clause *clause) {
  ClauseStrategy *strategy;
  if (typeid(*clause) == typeid(SuchThat)) {
    strategy = new SuchThatStrategy();
  } else if (typeid(*clause) == typeid(Pattern)) {
    strategy = new PatternStrategy();
  } else {
    strategy = new WithStrategy();
  }
  return strategy->DetermineClauseCommand(clause, group_table);
}

std::tuple<PKBQueryCommand *, ClauseCommand *>
ClauseStrategy::DetermineDoubleSynonymCommands(QueryEvaluatorTable *table, Synonym *first_synonym,
                                               Synonym *second_synonym, Clause *clause) {
  PKBQueryCommand *query_command = nullptr;
  if (typeid(*clause) == typeid(SuchThat)) {
    query_command = new QuerySuchThatTwoSynonymCommand(clause);
  } else if (typeid(*clause) == typeid(Pattern)) {
    query_command = new QueryPatternTwoSynonymCommand(clause);
  } else {
    query_command = new QueryWithTwoSynonymCommand(clause);
  }
  ClauseCommand *clause_command = nullptr;
  if (table->ContainsColumn(first_synonym) && table->ContainsColumn(second_synonym)) {
    clause_command = new DoubleSynonymBothPresentCommand();
  } else if (table->ContainsColumn(first_synonym)) {
    clause_command = new DoubleSynonymSinglePresentCommand(true);
  } else if (table->ContainsColumn(second_synonym)) {
    clause_command = new DoubleSynonymSinglePresentCommand(false);
  } else {
    clause_command = new DoubleSynonymNonePresentCommand();
  }
  return std::make_tuple(query_command, clause_command);
}

std::tuple<PKBQueryCommand *, ClauseCommand *>
ClauseStrategy::DetermineSingleSynonymCommand(QueryEvaluatorTable *table, bool synonym_is_first_param, Clause *clause) {
  PKBQueryCommand *query_command = nullptr;
  ClauseCommand *clause_command = nullptr;
  if (typeid(*clause) == typeid(SuchThat)) {
    query_command = new QuerySuchThatOneSynonymCommand(clause);
  } else if (typeid(*clause) == typeid(Pattern)) {
    query_command = new QueryPatternOneSynonymCommand(clause);
  } else {
    query_command = new QueryWithOneSynonymCommand(clause);
  }
  Synonym* synonym = synonym_is_first_param ? clause->first_synonym : clause->second_synonym;
  if (table->ContainsColumn(synonym)) {
    clause_command = new SingleSynonymPresentCommand(synonym_is_first_param);
  } else {
    clause_command = new SingleSynonymNonePresentCommand(synonym_is_first_param);
  }
  return std::make_tuple(query_command, clause_command);
}

/**
 * Determines the commands to be used during the pipeline of the clause evaluation.
 * @param clause The clause to be evaluated.
 * @param table
 * @return
 */
std::tuple<PKBQueryCommand *, ClauseCommand *>
SuchThatStrategy::DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) {
  auto* such_that_clause = dynamic_cast<SuchThat*>(clause);
  PKBQueryCommand *query_command = nullptr;
  ClauseCommand *clause_command = nullptr;

  if (such_that_clause->left_is_synonym && such_that_clause->right_is_synonym) {
    // Both are synonyms
    return DetermineDoubleSynonymCommands(table, such_that_clause->first_synonym, such_that_clause->second_synonym, clause);
  } else if (such_that_clause->left_is_synonym) {
    return DetermineSingleSynonymCommand(table, true, clause);
  } else if (such_that_clause->right_is_synonym) {
    return DetermineSingleSynonymCommand(table, false, clause);
  } else {
    query_command = new QuerySuchThatNoSynonymCommand(clause);
    return std::make_tuple(query_command, clause_command);
  }
}

std::tuple<PKBQueryCommand *, ClauseCommand *>
PatternStrategy::DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) {
  auto* pattern_clause = dynamic_cast<Pattern*>(clause);
  Synonym *assign_synonym = pattern_clause->first_synonym;
  PKBQueryCommand *query_command = nullptr;
  ClauseCommand *clause_command = nullptr;

  if (pattern_clause->left_is_synonym) {
    Synonym *variable_synonym = pattern_clause->second_synonym;
    return DetermineDoubleSynonymCommands(table, assign_synonym, variable_synonym, clause);
  } else {
    if (table->ContainsColumn(assign_synonym)) {
      return DetermineSingleSynonymCommand(table, true, clause);
    } else {
      return DetermineSingleSynonymCommand(table, true, clause);
    }
  }
  return std::make_tuple(query_command, clause_command);
}

std::tuple<PKBQueryCommand *, ClauseCommand *>
WithStrategy::DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) {
  auto* with_clause = dynamic_cast<With*>(clause);

  if (with_clause->left_is_synonym && with_clause->right_is_synonym) {
    return DetermineDoubleSynonymCommands(table, with_clause->first_synonym, with_clause->second_synonym, clause);
  } else if (with_clause->left_is_synonym) {
    return DetermineSingleSynonymCommand(table, true, clause);
  } else if (with_clause->right_is_synonym) {
    return DetermineSingleSynonymCommand(table, false, clause);
  } else {
    // This is handled by the queryEvaluator as a shortcut instead. (Optimisation)
    assert(false);
  }
}
