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
    explicit QueryEvaluatorTable(Synonym *target);
    explicit QueryEvaluatorTable(std::vector<Synonym *> target_list);

    // Add target synonym column with values to table
    bool AddTargetSynonymValues(Synonym *target, std::vector<Entity *> entity_list);
    // Add column to table
    bool AddColumn(Synonym *synonym);
    // Check if the table contains the column based on the synonym*
    virtual bool ContainsColumn(Synonym *synonym);
    // Add Rows to table
    bool AddMultipleRowForAllColumn(Synonym *synonym, int index, Entity *entity, int repeat_count);
    // Delete row
    bool DeleteRow(int index);
    // Return vector of specified synonym
    std::vector<Entity *> GetColumn(Synonym *synonym);
    // Return the number of columns
    int GetColumnSize();
    // Return the number of rows in the table
    int GetRowSize();
    // Return vector of target synonym results
    std::vector<std::vector<Entity*>> GetResults();
    // Return the vector of target synonyms in the table.
    std::vector<Synonym*> GetTargetSynonymList();
    // Add row (and new col)
    bool AddRow(Synonym *synonym, int index, Entity *entity);
    // Cross product a new synonyms' columns with the values in the table
    void CrossProductColumns(std::vector<Synonym *> synonyms, std::vector<std::vector<Entity *>> values);

  private:
    std::unordered_map<Synonym *, std::vector<Entity *>> synonym_to_entity_map;
    std::vector<Synonym *> target_synonym_list;

    void CrossProductSingleColumn(Synonym *synonym, const std::vector<Entity *>& values);
    void DuplicateTableForCrossProduct(int original_size);
    void InsertValuesForCrossProduct(Synonym *synonym_column, Entity *value, int original_size);
};

#endif //AUTOTESTER_QUERYEVALUATORTABLE_H
