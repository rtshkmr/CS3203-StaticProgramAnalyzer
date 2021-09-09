//
// Created by Max Ng on 6/9/21.
//

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
};

// Add target synonym column with values to table
bool QueryEvaluatorTable::AddTargetSynonym(std::list<std::string> synonym_list) {
  std::vector<std::string> synonym_vector;
  // TODO: Change internal pql API input to be a vector instead of a list to avoid this.
  for (std::string const &value : synonym_list) {
    synonym_vector.push_back(value);
  }
  um[target_synonym] = synonym_vector;
  return true;

};

// Delete row
bool QueryEvaluatorTable::DeleteRow(int index) {
  if (um[target_synonym].size() - 1 < index) {
    return false;
  }
  for ( auto iter = um.begin(); iter != um.end(); ++iter ) {
    std::vector<std::string> current_column = iter->second;
    int size = current_column.size();
    if (index < size) {
      current_column.erase(current_column.begin());
      iter->second = current_column;
    }
  }
  return true;
};

// Add row (and new col)
bool QueryEvaluatorTable::AddRow(std::string synonym, int index, std::string value) {
  // assert that index == size
  if (index != um[synonym].size()) {
    return false;
  }
  um[synonym].push_back(value);
  return true;
};

// Return vector of target synonym
std::vector<std::string> QueryEvaluatorTable::GetResults() {
  auto search = um.find(target_synonym);
  //assert search != um.end()
  return search->second;
};

// Return vector of specified synonym
std::vector<std::string> QueryEvaluatorTable::GetColumn(std::string synonym) {
  auto search = um.find(synonym);
  //assert search != um.end()
  return search->second;
};

// Empty column
bool QueryEvaluatorTable::RemoveColumn(std::string synonym) {
  auto search = um.find(synonym);
  if (search == um.end()) {
    return false;
  } else {
    search->second.clear();
    return true;
  }
};

int QueryEvaluatorTable::GetSize() {
  return um.size();
};

