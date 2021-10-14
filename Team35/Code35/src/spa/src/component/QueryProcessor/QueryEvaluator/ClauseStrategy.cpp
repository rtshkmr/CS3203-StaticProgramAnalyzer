//
// Created by Max Ng on 13/10/21.
//

#include "ClauseStrategy.h"

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
  } else {
    strategy = new PatternStrategy();
  }
  return strategy->DetermineClauseCommand(clause, group_table);
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
    Synonym *first_synonym = such_that_clause->first_synonym;
    Synonym *second_synonym = such_that_clause->second_synonym;
    query_command = new QuerySuchThatTwoSynonymCommand(clause);
    if (table->ContainsColumn(first_synonym) && table->ContainsColumn(second_synonym)) {
      clause_command = new DoubleSynonymBothPresentCommand();
    } else if (table->ContainsColumn(first_synonym)) {
      // Command for FirstSynonym In table
      clause_command = new DoubleSynonymSinglePresentCommand(true);
    } else if (table->ContainsColumn(second_synonym)) {
      // Command for SecondSynonym In table
      clause_command = new DoubleSynonymSinglePresentCommand(false);
    } else {
      // Neither synonym in the target table
      // Nothing should execute here, if the code comes here, maybe throw an error.
    }

  } else if (such_that_clause->left_is_synonym) {
    // Command for one synonym, and first in table
    query_command = new QuerySuchThatOneSynonymCommand(clause);
    clause_command = new SingleSynonymPresentCommand(true);
  } else if (such_that_clause->right_is_synonym) {
    // Command for one synonym, and second in table
    query_command = new QuerySuchThatOneSynonymCommand(clause);
    clause_command = new SingleSynonymPresentCommand(false);
  } else {
    query_command = new QuerySuchThatNoSynonymCommand(clause);
  }

  return std::make_tuple(query_command, clause_command);
}

std::tuple<PKBQueryCommand *, ClauseCommand *>
PatternStrategy::DetermineClauseCommand(Clause *clause, QueryEvaluatorTable *table) {
  auto* pattern_clause = dynamic_cast<Pattern*>(clause);
  Synonym *assign_synonym = pattern_clause->first_synonym;
  PKBQueryCommand *query_command = nullptr;
  ClauseCommand *clause_command = nullptr;

  if (pattern_clause->left_is_synonym) {
    // Case for 2 synonyms
    Synonym *variable_synonym = pattern_clause->second_synonym;
    query_command = new QueryPatternTwoSynonymCommand(clause);
    if (table->ContainsColumn(assign_synonym) && table->ContainsColumn(variable_synonym)) {
      // Both synonym in table
      clause_command = new DoubleSynonymBothPresentCommand();
    } else if (table->ContainsColumn(assign_synonym)) {
      // Table only contains assign synonym
      clause_command = new DoubleSynonymSinglePresentCommand(true);
    } else if (table->ContainsColumn(variable_synonym)) {
      // Table only contains variable synonym
      clause_command = new DoubleSynonymSinglePresentCommand(false);
    } else {
      // Table contains no synonym
      // Technically this is not possible
    }
  } else {
    query_command = new QueryPatternOneSynonymCommand(clause);
    if (table->ContainsColumn(assign_synonym)) {
      clause_command = new SingleSynonymPresentCommand(true);
    } else {
      // Technically this should never run
    }
  }
}
