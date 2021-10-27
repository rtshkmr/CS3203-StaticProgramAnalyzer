#ifndef AUTOTESTER_QUERYEVALUATORTABLE_H
#define AUTOTESTER_QUERYEVALUATORTABLE_H

//#include <synonym_to_entity_map>
//#include <string>
//#include <vector>
//#include <utility>
//#include <model/Entity.h>
//#include "Types.h"

#include <model/Entity.h>
#include <unordered_map>
#include "Types.h"

class QueryEvaluatorTable {
  public:
    QueryEvaluatorTable(Synonym *target);
    QueryEvaluatorTable(std::vector<Synonym *> target_list);

    // Add target synonym column with values to table
    bool AddTargetSynonymValues(Synonym *target, std::vector<Entity *> entity_list);
    // Add column to table
    bool AddColumn(Synonym *synonym);
    bool ContainsColumn(Synonym *synonym);
    // Add Rows to table
    bool AddMultipleRowForAllColumn(Synonym *synonym, int index, Entity *entity, int repeat_count);
    // Delete row
    bool DeleteRow(int index);
    // Return vector of specified synonym
    std::vector<Entity *> GetColumn(Synonym *synonym);
    int GetColumnSize();
    int GetRowSize();

    // Return vector of target synonym results
    std::vector<std::vector<Entity*>> GetResults();
    std::vector<Synonym*> GetTargetSynonymList();
    // Add row (and new col)
    bool AddRow(Synonym *synonym, int index, Entity *entity);

  private:
    std::unordered_map<Synonym *, std::vector<Entity *>> synonym_to_entity_map;
    std::vector<Synonym *> target_synonym_list;

};

#endif //AUTOTESTER_QUERYEVALUATORTABLE_H
