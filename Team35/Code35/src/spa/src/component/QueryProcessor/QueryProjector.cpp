#include <list>
#include <cassert>
#include <model/Statement.h>
#include "QueryProjector.h"

QueryProjector::QueryProjector(std::vector<std::pair<Synonym*, Attribute>> target_syn_attr_list)
    : target_syn_attr_list(target_syn_attr_list) {
  for (auto p : target_syn_attr_list) {
    if (syn_to_attrs_map.find(p.first->GetName()) == syn_to_attrs_map.end()) {
      syn_to_attrs_map[p.first->GetName()] = std::vector<Attribute>();
    }
    syn_to_attrs_map[p.first->GetName()].push_back(p.second);
  }
}

/**
 * Formats the UnformattedQueryResult into the requirements for the TestWrapper.
 * First checks if boolean result is false and returns an empty list
 * Then checks if target is BOOLEAN and returns BOOLEAN accordingly.
 * Then gets results from all QueryEvaluatorTables in UnformattedQueryResult.
 * Then stringifies each table, cross product, reorder, and join into tuples for all tables if there is more than 1 table.
 *
 * Assumes that every Table has at least 1 target synonym, and can return the columns of target synonyms it contains.
 *
 * @param unformatted_results
 * @return Strings that the TestWrapper expects.
 */
std::vector<std::string> QueryProjector::FormatQuery(UnformattedQueryResult unformatted_results) {
  std::vector<QueryEvaluatorTable*> table_references = unformatted_results.GetTables();

  if (!unformatted_results.GetBooleanResult() && !target_syn_attr_list.empty()) {
    return std::vector<std::string>{};
  }
  // boolean synonym
  if (target_syn_attr_list.empty()) {
    std::vector<std::string> wrapped_boolean =
        std::vector<std::string>{unformatted_results.GetBooleanResult() ? "TRUE" : "FALSE"};
    return wrapped_boolean;
  }

  std::vector<Synonym*> result_synonym_order = {}; // order of synonyms within results before cross product
  std::vector<std::vector<std::vector<std::string>>> unordered_results = {};  // vector of tables of columns
  for (QueryEvaluatorTable* table : table_references) {
    // Get the unique list of results for that table
    std::vector<Synonym*> table_target_list = table->GetTargetSynonymList();
    std::vector<std::vector<Entity*>> temp_table = table->GetResults();
    std::vector<std::pair<Synonym*, Attribute>> table_target_syn_list;
    std::vector<std::vector<Entity*>> entity_table;
    for (int i = 0; i < table_target_list.size(); i++) {
      auto t = table_target_list[i];
      auto column = temp_table[i];
      auto attrs = syn_to_attrs_map[t->GetName()];
      for (auto a : attrs) {
        table_target_syn_list.push_back({t, a});
        entity_table.push_back(column);
      }
    }
    for (auto item : table_target_syn_list) {
      result_synonym_order.push_back(item.first);
    }
    std::vector<std::vector<std::string>> stringified_table = StringifyTable(table_target_syn_list, entity_table);
    unordered_results.push_back(stringified_table);
  }

  std::vector<std::string> query_ans;
  if (target_syn_attr_list.size() == 1) {
    query_ans = unordered_results.front()[0];
  } else {
    // need to cross product for multiple synonyms.
    query_ans = FormatMultipleTables(unordered_results, result_synonym_order, target_syn_attr_list);
  }
  query_ans.erase(std::unique(query_ans.begin(), query_ans.end()), query_ans.end());
  return query_ans;
}

/**
 * Stringifies each Entity in the entity_table according to its DesignEntity type as required by SPA.
 *
 * @param synonyms Vector of Synonyms that represent the type of each column of the table.
 * @param entity_table Vector of vector of Entities.
 * @return Stringified table.
 */
std::vector<std::vector<std::string>> QueryProjector::StringifyTable(std::vector<std::pair<Synonym*, Attribute>> syn_attrs,
                                                                     std::vector<std::vector<Entity*>> entity_table) {
  std::vector<std::vector<std::string>> stringified_table;
  for (int i = 0; i < syn_attrs.size(); ++i) {
    std::vector<std::string> stringified_column;
    std::vector<Entity*> entity_column = entity_table[i];
    switch (syn_attrs[i].second) {
      case Attribute::kStmtNumber:
        for (Entity* entity: entity_column) {
          int statement_num = dynamic_cast<Statement*>(entity)->GetStatementNumber();
          stringified_column.push_back(std::to_string(statement_num));
        }
        break;
      case Attribute::kVarName:
        for (Entity* entity: entity_column) {
          std::string var_string;
          Variable* temp;
          if (syn_attrs[i].first->GetType() == DesignEntity::kPrint) {
            temp = dynamic_cast<PrintEntity*>(entity)->GetVariableObj();
          }
          else if (syn_attrs[i].first->GetType() == DesignEntity::kRead) {
            temp = dynamic_cast<ReadEntity*>(entity)->GetVariableObj();
          }
          else {
            temp = dynamic_cast<Variable*>(entity);
          }
          var_string = temp->GetNameInString();
          stringified_column.push_back(var_string);
        }
        break;
      case Attribute::kProcName:
        for (Entity* entity: entity_column) {
          std::string proc_string;
          Procedure* temp;
          if (syn_attrs[i].first->GetType() == DesignEntity::kCall) {
            temp = dynamic_cast<CallEntity*>(entity)->GetCalledProcedure();
          } else {
            temp = dynamic_cast<Procedure*>(entity);
          }
          proc_string = const_cast<ProcedureName*>(temp->GetName())->GetName();
          stringified_column.push_back(proc_string);
        }
        break;
      case Attribute::kValue:
        for (Entity* entity: entity_column) {
          int constant_int = const_cast<ConstantValue*>(dynamic_cast<Constant*>(entity)->GetValue())->GetValue();
          stringified_column.push_back(std::to_string(constant_int));
        }
        break;
      case Attribute::kInvalid: break;
    }
    stringified_table.push_back(stringified_column);
  }
  return stringified_table;
}

std::vector<std::string> QueryProjector::FormatMultipleTables(std::vector<std::vector<std::vector<std::string>>> tables,
                                                              std::vector<Synonym*> table_synonym_order,
                                                              std::vector<std::pair<Synonym*, Attribute>> tgt_syn_attrs) {
  std::vector<std::vector<std::string>> crossed_table = tables[0];  // vector of tables of columns
  for (int i = 1; i < tables.size(); ++i) {
    std::vector<std::vector<std::string>> current_table = tables[i];
    crossed_table = CrossProductTables(crossed_table, current_table);
  }

  std::vector<std::vector<std::string>> ordered_table = ReorderTable(std::move(tgt_syn_attrs),
                                                                     std::move(table_synonym_order),
                                                                     crossed_table);
  std::vector<std::string> query_ans = JoinTuples(ordered_table);
  return query_ans;
};

/**
 * Makes all combinations of the rows of the 2 tables into 1.
 * Assumes that all columns in 1 table has the same number of rows.
 * Ensures that rows within a table will stay in their column-wise order.
 *
 * @param t1
 * @param t2
 * @return Table of all combinations.
 */
std::vector<std::vector<std::string>> QueryProjector::CrossProductTables(std::vector<std::vector<std::string>> t1,
                                                                         std::vector<std::vector<std::string>> t2) {
  std::vector<std::vector<std::string>> crossed_table(t1.size() + t2.size());

  for (int t1row = 0; t1row < t1[0].size(); ++t1row) {
    for (int t2row = 0; t2row < t2[0].size(); ++t2row) {
      for (int t1col = 0; t1col < t1.size(); ++t1col) {
        crossed_table[t1col].push_back(t1[t1col][t1row]);
      }
      for (int t2col = 0; t2col < t2.size(); ++t2col) {
        crossed_table[t1.size() + t2col].push_back(t2[t2col][t2row]);
      }
    }
  }
  return crossed_table;
}

/**
 * Reorders the columns of the table according to the desired synonym order, comparing using synonym ==.
 * Synonyms from the Table and recorded order should have the same pointer, unless there multiple similar synonyms.
 * Assumes that when the desired order and current order does not have the same size, it means there are
 * multiple similar synonyms.
 *
 * @param desired_order List of synonyms in the desired order.
 * @param current_order List of synonyms in the current order.
 * @param table Table to reorder.
 * @return Reordered table.
 */
std::vector<std::vector<std::string>> QueryProjector::ReorderTable(std::vector<std::pair<Synonym*, Attribute>> desired_order,
                                                                   std::vector<Synonym*> current_order,
                                                                   std::vector<std::vector<std::string>> table) {
  std::vector<std::vector<std::string>> reordered_table(desired_order.size());

  int i = 0;
  for (auto p: desired_order) {
    Synonym* syn = p.first;
    if (i < current_order.size() && *current_order[i] == *syn) {
      reordered_table[i] = table[i];
    } else {
      int current_index = -1;
      for (int _t = 0; _t < current_order.size(); _t++) {
        if (*current_order[_t] == *syn) {
          current_index = _t;
          break;
        };
      }
      assert(current_index != -1);
      reordered_table[i] = table[current_index];
    }
    i++;
  }
  return reordered_table;
}

/**
 * Joins the rows of the table into a string in the format of a Tuple described by the requirements of SPA.
 *
 * @param table
 * @return Vector of string tuples.
 */
std::vector<std::string> QueryProjector::JoinTuples(std::vector<std::vector<std::string>> table) {
  std::vector<std::string> tuples;
  for (int row = 0; row < table[0].size(); ++row) {
    std::string tuple;
    for (int col = 0; col < table.size(); ++col) {
      tuple.append(table[col][row] + " ");
    }
    tuple.pop_back(); // removes last space, and it is assumed that there must be some value in the tuple.
    tuples.push_back(tuple);
  }
  return tuples;
}

