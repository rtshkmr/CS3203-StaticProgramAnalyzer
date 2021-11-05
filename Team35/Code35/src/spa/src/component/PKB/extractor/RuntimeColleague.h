#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMECOLLEAGUE_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMECOLLEAGUE_H_

#include <model/Entity.h>
#include <component/QueryProcessor/types/Types.h>

enum class RelDirection {
  kForward,
  kReverse
};

/**
 * Colleague interface for extractors called by RuntimeMediator.
 */
class RuntimeColleague {
 public:
  virtual std::vector<Entity*> GetRelationship(RelDirection dir, int target) = 0;
  virtual std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir, DesignEntity de) = 0;
  virtual std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir,
                                                                           DesignEntity first,
                                                                           DesignEntity second) = 0;
  virtual bool HasRelationship(RelDirection dir) = 0;
  virtual bool HasRelationship(RelDirection dir, int target) = 0;
  virtual bool HasRelationship(RelDirection dir, int first, int second) = 0;
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMECOLLEAGUE_H_
