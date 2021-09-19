#include <util/Logger.h>

#include <utility>
#include "QueryEvaluator.h"

/**
 * Processes the group list containing the information of the query. This method will then make the relevant calls to
 * various methods to further process each group of queries.
 * @param syn_list The list of synonyms declared in the query declaration.
 * @param target The target synonym of the Select clause.
 * @param groups The list of groups extracted from the query provided.
 * @param pkb The populated PKB database.
 */
QueryEvaluator::QueryEvaluator(std::list<Synonym> syn_list, Synonym target, std::list<Group*> groups, PKB pkb)
: synonym_list{std::move(syn_list)}, target_synonym{std::move(target)}, group_list{std::move(groups)}, pkb{std::move(pkb)}, boolean_result{true},
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
    for (auto iter : synonym_list) {
        DesignEntity synonym_design_entity = iter.GetType();
        std::string synonym_name = iter.GetName();
        synonym_design_entity_map[synonym_name] = synonym_design_entity;
        std::list<std::string> list_of_synonym_values = pkb.GetDesignEntity(synonym_design_entity);
        map_of_synonym_values[synonym_name] = list_of_synonym_values;

        if (iter.GetName() == target_synonym.GetName()) {
            table->AddTargetSynonym(list_of_synonym_values);
        }
    }
}

/**
 * For each group, they may be boolean or not; each group will be processed according to whether they are boolean or not.
 * @param table The current table in question for the boolean or non-boolean group.
 */
void QueryEvaluator::EvaluateAllGroups(QueryEvaluatorTable* table) {
    // For each group, evaluate the group with target synonym first if applicable
    for (auto iter : group_list) {
        std::vector<Clause*> clause_list = (*iter).GetClauses();

        if ((*iter).ContainsTargetSynonym()) {
        // This Group contains the target synonym
          ProcessGroup(clause_list, table);
        } else {
          // This Group contains no target synonym and is treated as a boolean type group
          // Each boolean group has their own table.
          PreprocessBooleanGroup(clause_list);
        }
    }
}

/**
 * Determing the result of the query by factoring in the result of the boolean groups.
 * @param table The table containing the target synonym.
 * @return a list of strings representing the final correct values.
 */
std::vector<std::string> QueryEvaluator::GetResult(QueryEvaluatorTable* table) const {
  if (!boolean_result) {
    std::vector<std::string> empty_list = {};
    return empty_list;
  } else {
    return table->GetResults();
  }
}

/**
 * Process each group based on whether the contain the target synonym or not.
 * @param clause_list The list of clauses for a particular group.
 * @param table The table representing a particular group.
 */
void QueryEvaluator::ProcessGroup(const std::vector<Clause*>& clause_list, QueryEvaluatorTable* table) {

    for (Clause* current_clause : clause_list) {

        if (typeid(*current_clause) == typeid(SuchThat)) {
            // Evaluate such that clause here
            auto* st = dynamic_cast<SuchThat*>(current_clause);
            EvaluateSuchThatClause(*st, table, pkb, synonym_list);
        } else {
            // Evaluate pattern clause here
            auto* p = dynamic_cast<Pattern*>(current_clause);
            ProcessPatternClause(*p, table, pkb, synonym_design_entity_map);
        }
    }
}

/**
 * Finds the main synonym for a boolean group and adding it to the table representing the current boolean group.
 * @param clause_list The list of clause in the current group.
 */
void QueryEvaluator::PreprocessBooleanGroup(std::vector<Clause*> clause_list) {
    Clause* firstClause = clause_list[0];
    std::string synonym_name;
    if (typeid(*firstClause) == typeid(SuchThat)) {
        auto* st = dynamic_cast<SuchThat*>(firstClause);
        if (st->left_is_synonym) {
            synonym_name = st->left_hand_side;
        } else if (st->right_is_synonym) {
            synonym_name = st->right_hand_side;
        } else {
            // If no synonym, no need for a table. Also, should be in a group of size 1.
            bool result = EvaluateNoSynonym(*st, pkb);
            if (!result) {
              boolean_result = false;        // Call to the class variable directly
            }
        }
    } else if (typeid(*firstClause) == typeid(Pattern)) {
        auto* pattern = dynamic_cast<Pattern*>(firstClause);
        synonym_name = pattern->assign_synonym;
    } else {
        // No code should run here for iter 1 since there is only such that and pattern clause.
    }

    if (!synonym_name.empty()) {
      QueryEvaluatorTable current_table(synonym_name);
      current_table.AddTargetSynonym(map_of_synonym_values[synonym_name]);
      ProcessGroup(clause_list, &current_table);
      if (current_table.GetResults().empty()) {
        boolean_result = false;
      }
    }
}
