#include <util/Logger.h>
#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group*> groups, PKB pkb)
: synonym_list{syn_list}, target_synonym{target}, group_list{groups}, pkb{pkb}, boolean_result{true},
  map_of_synonym_values{}, synonym_design_entity_map() {}

std::vector<std::string> QueryEvaluator::EvaluateQuery() {

    QueryEvaluatorTable target_synonym_table(target_synonym.GetName());

    PopulateSynonymValues(&target_synonym_table);
    EvaluateAllGroups(&target_synonym_table);

    // Check the boolean conditions and return the final results
    return GetResult(&target_synonym_table);
}

/**
 * Stores the information for each synonym declared in an unordered map by storing all of its values and it's design
 * entity.
 * @param table The QueryEvaluatorTable that will be storing the target synonym.
 */
void QueryEvaluator::PopulateSynonymValues(QueryEvaluatorTable* table) {
    // Query all the design entities and add them to an unordered_map.
    for (auto iter = synonym_list.begin(); iter != synonym_list.end(); iter++) {
        DesignEntity synonym_design_entity = iter->GetType();
        std::string synonym_name = iter->GetName();
        synonym_design_entity_map[synonym_name] = synonym_design_entity;
        std::list<std::string> list_of_synonym_values = pkb.GetDesignEntity(synonym_design_entity);
        map_of_synonym_values[synonym_name] = list_of_synonym_values;

        if (iter->GetName() == target_synonym.GetName()) {
            table->AddTargetSynonym(list_of_synonym_values);
        }
    }
}

void QueryEvaluator::EvaluateAllGroups(QueryEvaluatorTable* table) {
    // For each group, evaluate the group with target synonym first if applicable
    for (auto iter = group_list.begin(); iter != group_list.end(); iter++) {
        std::vector<Clause*> clause_list = (*iter)->GetClauses();

        if ((*iter)->ContainsTargetSynonym()) {
        // This Group contains the target synonym
          ProcessGroup(clause_list, table);
        } else {
          // This Group contains no target synonym and is treated as a boolean type group
          // Each boolean group has their own table.
          PreprocessBooleanGroup(clause_list);
        }
    }
}

std::vector<std::string> QueryEvaluator::GetResult(QueryEvaluatorTable* table) {
  if (!boolean_result) {
    std::vector<std::string> empty_list = {};
    return empty_list;
  } else {
    return table->GetResults();
  }
}

void QueryEvaluator::ProcessGroup(std::vector<Clause*> clause_list, QueryEvaluatorTable* table) {

    for (auto iter = clause_list.begin(); iter != clause_list.end(); iter++) {
        Clause* currentClause = *iter;

        if (typeid(*currentClause) == typeid(SuchThat)) {
            // Evaluate such that clause here
            SuchThat* st = dynamic_cast<SuchThat*>(currentClause);
            EvaluateSuchThatClause(*st, table, pkb, synonym_list);
        } else {
            // Evaluate pattern clause here
            Pattern* p = dynamic_cast<Pattern*>(currentClause);
            ProcessPatternClause(*p, table, pkb, synonym_design_entity_map);
        }
    }
}

void QueryEvaluator::PreprocessBooleanGroup(std::vector<Clause*> clause_list) {
    Clause* firstClause = clause_list[0];
    std::string synonym_name = "";
    if (typeid(*firstClause) == typeid(SuchThat)) {
        SuchThat* st = dynamic_cast<SuchThat*>(firstClause);
        if (st->left_is_synonym) {
            synonym_name = st->left_hand_side;
        } else if (st->right_is_synonym) {
            synonym_name = st->right_hand_side;
        } else {
            // If no synonym, no need for a table. Also, should be in a group of size 1.
            bool result = EvaluateNoSynonym(*st, pkb);
            if (result == false) {
              boolean_result = false;        // Call to the class variable directly
            }
        }
    } else if (typeid(*firstClause) == typeid(Pattern)) {
        Pattern* pattern = dynamic_cast<Pattern*>(firstClause);
        synonym_name = pattern->assign_synonym;
    } else {
        // No code should run here for iter 1 since there is only such that and pattern clause.
    }

    if (synonym_name != "") {
      QueryEvaluatorTable current_table(synonym_name);
      current_table.AddTargetSynonym(map_of_synonym_values[synonym_name]);
      ProcessGroup(clause_list, &current_table);
      if (current_table.GetResults().size() == 0) {
        boolean_result = false;
      }
    }
}