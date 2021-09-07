//
// Created by Max Ng on 6/9/21.
//

#include "QueryEvaluatorTable.h"

QueryEvaluatorTable::QueryEvaluatorTable(std::string target) {
  targetSynonym = target;
}

// Add column to table
bool QueryEvaluatorTable::addColumn(std::string synonym) {
  if (um.find(synonym) == um.end()) {
    std::vector<std::string> synonymList;
    um.insert(std::make_pair(synonym, synonymList));
  }
};

// Delete row
bool QueryEvaluatorTable::deleteRow(int index) {
  if (um.size() - 1 < index) {
    return false;
  }
  for ( auto iter = um.begin(); iter != um.end(); ++iter ) {
    std::vector<std::string> currentColumn = iter->second;
    int size = currentColumn.size();
    if (index < size) {
      currentColumn.erase(currentColumn.begin() + index - 1);
    }
  }
  return true;
};

// Add row (and new col)
bool QueryEvaluatorTable::addRow(std::string synonym, int index, std::string value) {
  // assert that index == size + 1
  um[synonym].push_back(value);
  return true;
};

// Return vector of target synonym
std::vector<std::string> QueryEvaluatorTable::getResults() {
  auto search = um.find(targetSynonym);
  //assert search != um.end()
  return search->second;
};

// Empty column
bool QueryEvaluatorTable::removeColumn(std::string synonym) {
  auto search = um.find(synonym);
  if (search == um.end()) {
    return false;
  } else {
    search->second.clear();
    return true;
  }
};

int QueryEvaluatorTable::getSize() {
  return um.size();
};

