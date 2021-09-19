#include "QueryEvaluatorTable.h"

QueryEvaluatorTable::QueryEvaluatorTable(std::string target) {
  target_synonym = target;
}

// Add column to table
bool QueryEvaluatorTable::AddColumn(std::string synonym) {
  if (um.find(synonym) == um.end()) {
    std::vector<std::string> synonym_list;
    um.insert(std::make_pair(synonym, synonym_list));
    return true;
  } else {
    return false;
  }
}

// Add target synonym column with values to table
bool QueryEvaluatorTable::AddTargetSynonym(std::list<std::string> synonym_list) {
  std::vector<std::string> synonym_vector;
  // TODO: Change internal pql API input to be a vector instead of a list to avoid this.
  for (std::string const& value: synonym_list) {
    synonym_vector.push_back(value);
  }
  um[target_synonym] = synonym_vector;
  return true;

}

// Delete row
bool QueryEvaluatorTable::DeleteRow(int index) {
  if (um[target_synonym].size() - 1 < index) {
    return false;
  }
  for (auto iter = um.begin(); iter != um.end(); ++iter) {
    std::vector<std::string> current_column = iter->second;
    unsigned long size = current_column.size();
    if (index < size) {
      current_column.erase(current_column.begin() + index);
      iter->second = current_column;
    }
  }
  return true;
}

// Add row (and new col)
bool QueryEvaluatorTable::AddRow(std::string synonym, int index, std::string value) {
  // assert that index == size
  if (index != um[synonym].size()) {
    return false;
  }
  um[synonym].push_back(value);
  return true;
}

bool QueryEvaluatorTable::AddRowForAllColumn(std::string synonym, int index, std::string value) {
  for (auto tableIterator = um.begin(); tableIterator != um.end(); tableIterator++) {
    if (tableIterator->first == synonym) {
      AddRow(synonym, index, value);
    } else {
      std::vector<std::string> currList = tableIterator->second;
      currList.insert(currList.begin() + index, currList[index]);
      tableIterator->second = currList;
    }
  }
  return true;
}

/**
 * The first time a row is added (i.e repeat_count = 0), the value is added to the column under 'synonym'.
 * Each subsequent time a row is added (i.e repeat_count > 0), the value for all other rows are copied, and the value
 * is added to the column under 'synonym'.
 *
 * @param synonym The name of the synonym which acts as the column header.
 * @param index The position to insert the row in the table. This should be the same across repeated rows.
 * @param value The value to be inserted into the column.
 * @param repeat_count The number of times this call has been repeated for the same values in all other columns.
 * @return false if there is an error with adding values into the table or if the synonym column does not exist.
 * True otherwise.
 */
bool QueryEvaluatorTable::AddMultipleRowForAllColumn(std::string synonym,
                                                     int index,
                                                     std::string value,
                                                     int repeat_count) {
  if (!ContainsColumn(synonym)) {
    return false;
  }

  for (auto tableIterator = um.begin(); tableIterator != um.end(); tableIterator++) {
    if (tableIterator->first == synonym) {
      AddRow(synonym, index + repeat_count, value);
    } else {
      if (repeat_count > 0) {
        std::vector<std::string> currList = tableIterator->second;
        currList.insert(currList.begin() + index + repeat_count, currList[index]);
        tableIterator->second = currList;
      }
    }
  }
  return true;
}

// Return vector of target synonym
std::vector<std::string> QueryEvaluatorTable::GetResults() {
  auto search = um.find(target_synonym);
  //assert search != um.end()
  return search->second;
}

// Return vector of specified synonym
std::vector<std::string> QueryEvaluatorTable::GetColumn(std::string synonym) {
  auto search = um.find(synonym);
  //assert search != um.end()
  return search->second;
}

// Empty column
bool QueryEvaluatorTable::RemoveColumn(std::string synonym) {
  auto search = um.find(synonym);
  if (search == um.end()) {
    return false;
  } else {
    search->second.clear();
    return true;
  }
}

bool QueryEvaluatorTable::ContainsColumn(std::string synonym) {
  auto search = um.find(synonym);
  return search != um.end();
}

int QueryEvaluatorTable::GetSize() {
  return um.size();
}

int QueryEvaluatorTable::GetRowSize() {
  auto search = um.find(target_synonym);
  //assert search != um.end()
  return search->second.size();
}

std::string QueryEvaluatorTable::GetStatementSynonym(std::unordered_map<std::string,
                                                                        DesignEntity> synonym_design_entity_map) {
  bool is_statement = false;
  for (auto tableIterator = um.begin(); tableIterator != um.end(); tableIterator++) {
    DesignEntity current_design_entity = synonym_design_entity_map[tableIterator->first];
    switch (current_design_entity) {
      case DesignEntity::kStmt:
      case DesignEntity::kAssign:
      case DesignEntity::kCall:
      case DesignEntity::kPrint:
      case DesignEntity::kRead:
      case DesignEntity::kIf:
      case DesignEntity::kWhile:is_statement = true;
        break;
      default:break;
    }
    if (is_statement) {
      return tableIterator->first;
    }
  }
  return "";
}
