#include "QueryEvaluatorTable.h"

bool QueryEvaluatorTable::AddTargetSynonymValues(Synonym *target, std::vector<Entity *> entity_list) {
  synonym_to_entity_map[target] = entity_list;
  return true;
}


bool QueryEvaluatorTable::AddColumn(Synonym *synonym) {
  if (synonym_to_entity_map.find(synonym) == synonym_to_entity_map.end()) {
    std::vector<Entity *> synonym_list;
    synonym_to_entity_map.insert(std::make_pair(synonym, synonym_list));
    return true;
  } else {
    return false;
  }
}

bool QueryEvaluatorTable::ContainsColumn(Synonym *synonym) {
  auto search = synonym_to_entity_map.find(synonym);
  return search != synonym_to_entity_map.end();
}

bool QueryEvaluatorTable::AddRow(Synonym *synonym, int index, Entity *entity) {
  // assert that index == size
  if (index != synonym_to_entity_map[synonym].size()) {
    return false;
  }
  synonym_to_entity_map[synonym].push_back(entity);
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

  for (auto tableIterator = synonym_to_entity_map.begin(); tableIterator != synonym_to_entity_map.end(); tableIterator++) {
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
  if (synonym_to_entity_map[target_synonym_list[0]].size() - 1 < index) {
    return false;
  }
  for (auto iter = synonym_to_entity_map.begin(); iter != synonym_to_entity_map.end(); ++iter) {
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
  auto search = synonym_to_entity_map.find(synonym);
  //assert search != synonym_to_entity_map.end()
  return search->second;
}

int QueryEvaluatorTable::GetColumnSize() {
  return synonym_to_entity_map.size();
}

/**
 * The row size of the table is calculated from the number of rows for the first target synonym. This is because
 * the table is assumed to contain the first row first.
 * @return
 */
int QueryEvaluatorTable::GetRowSize() {
  auto search = synonym_to_entity_map.find(target_synonym_list[0]);

  if(search == synonym_to_entity_map.end()) return 0;
  return search->second.size();
}

/**
 * Gets a vector of columns in this table, whose column headers(synonyms) are in the target synonym list.
 * Order of columns will be kept the same as the target synonym list.
 *
 * @return Vector of columns which are vectors of Entity*
 */
std::vector<std::vector<Entity*>> QueryEvaluatorTable::GetResults() {
  std::vector<std::vector<Entity *>> target_table;
  for (auto synonym : target_synonym_list) {
    if (synonym_to_entity_map.count(synonym) > 0) {
      std::vector<Entity*> column = synonym_to_entity_map.find(synonym)->second;
      target_table.push_back(column);
    }
  }

  return target_table;
}

std::vector<Synonym*> QueryEvaluatorTable::GetTargetSynonymList() {
  return target_synonym_list;
}

QueryEvaluatorTable::QueryEvaluatorTable(Synonym *target) {
  std::vector<Synonym *> list = {target};
  target_synonym_list = list;
}

QueryEvaluatorTable::QueryEvaluatorTable(std::vector<Synonym *> target_list) {
  target_synonym_list = target_list;
}
