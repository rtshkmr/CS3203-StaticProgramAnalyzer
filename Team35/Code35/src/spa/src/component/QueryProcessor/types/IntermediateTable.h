#ifndef AUTOTESTER_INTERMEDIATETABLE_H
#define AUTOTESTER_INTERMEDIATETABLE_H

#include <vector>
#include <model/Entity.h>

class IntermediateTable {
  private:
    std::vector<Entity *> relationships;
    std::vector<std::tuple<Entity *, Entity *>> relationships_by_type;
    bool has_relationship;

  public:
    IntermediateTable();
    void InsertData(std::vector<Entity *> data);
    void InsertData(std::vector<std::tuple<Entity *, Entity *>> data);
    void InsertData(bool has_relationship);
    std::vector<Entity *> GetRelationships();
    std::vector<std::tuple<Entity *, Entity *>> GetRelationshipsByType();
    bool GetExistenceResult();
};

#endif //AUTOTESTER_INTERMEDIATETABLE_H
