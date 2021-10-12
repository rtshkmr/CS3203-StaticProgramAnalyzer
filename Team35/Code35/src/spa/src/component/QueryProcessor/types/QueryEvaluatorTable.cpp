#include "QueryEvaluatorTable.h"

bool QueryEvaluatorTable::AddTargetSynonymValues(std::vector<Entity *> entity_list) {
  unordered_map[target_synonym] = entity_list;
  return true;
}


bool QueryEvaluatorTable::AddColumn(Synonym *synonym) {
  if (unordered_map.find(synonym) == unordered_map.end()) {
    std::vector<Entity *> synonym_list;
    unordered_map.insert(std::make_pair(synonym, synonym_list));
    return true;
  } else {
    return false;
  }
}

bool QueryEvaluatorTable::ContainsColumn(Synonym *synonym) {
  auto search = unordered_map.find(synonym);
  return search != unordered_map.end();
}

bool QueryEvaluatorTable::AddRow(Synonym *synonym, int index, Entity *entity) {
  // assert that index == size
  if (index != unordered_map[synonym].size()) {
    return false;
  }
  unordered_map[synonym].push_back(entity);
  return true;
}

/**
 * The first time a row is added (i.e repeat_count = 0), the value is added to the column under 'synonym'.
 * Each subsequent time a row is added (i.e repeat_count > 0), the value for all other rows are copied, and the value
 * is added to the column under 'synonym'.
 *
 * @param synonym
 * @param index
 * @param entity
 * @param count
 * @return
 */
bool QueryEvaluatorTable::AddMultipleRowForAllColumn(Synonym *synonym, int index, Entity *entity, int repeat_count) {
  if (!ContainsColumn(synonym)) {
    return false;
  }

  for (auto tableIterator = unordered_map.begin(); tableIterator != unordered_map.end(); tableIterator++) {
    if (tableIterator->first == synonym) {
      AddRow(synonym, index + repeat_count, entity);
    } else {
      if (repeat_count > 0) {
        std::vector<Entity *> currList = tableIterator->second;
        currList.insert(currList.begin() + index + repeat_count, currList[index]);
        tableIterator->second = currList;
      }
    }
  }
  return true;
}

bool QueryEvaluatorTable::DeleteRow(int index) {
  if (unordered_map[target_synonym].size() - 1 < index) {
    return false;
  }
  for (auto iter = unordered_map.begin(); iter != unordered_map.end(); ++iter) {
    std::vector<Entity *> current_column = iter->second;
    unsigned long size = current_column.size();
    if (index < size) {
      current_column.erase(current_column.begin() + index);
      iter->second = current_column;
    }
  }
  return true;
}

std::vector<Entity *> QueryEvaluatorTable::GetColumn(Synonym *synonym) {
  auto search = unordered_map.find(synonym);
  //assert search != unordered_map.end()
  return search->second;
}

int QueryEvaluatorTable::GetColumnSize() {
  return unordered_map.size();
}

int QueryEvaluatorTable::GetRowSize() {
  auto search = unordered_map.find(target_synonym);
  //assert search != unordered_map.end()
  return search->second.size();
}

std::vector<Entity *> QueryEvaluatorTable::GetResults() {
  auto search = unordered_map.find(target_synonym);
  //assert search != unordered_map.end()
  return search->second;
}

QueryEvaluatorTable::QueryEvaluatorTable(Synonym *target) {
  target_synonym = target;
  std::vector<Synonym *> list = {target};
  target_synonym_list = list;
}

QueryEvaluatorTable::QueryEvaluatorTable(std::vector<Synonym *> target_list) {
  target_synonym = target_list[0];
  target_synonym_list = target_list;
}

