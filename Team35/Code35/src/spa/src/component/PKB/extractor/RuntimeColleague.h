#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMECOLLEAGUE_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMECOLLEAGUE_H_

#include <model/Entity.h>

enum class RelDirection {
  kForward,
  kReverse
};

/**
 * Colleague interface for extractors called by RuntimeMediator.
 */
class RuntimeColleague {
 public:
  virtual std::vector<Entity*> GetRelationship(RelDirection dir, std::string target) = 0;
  virtual std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir) = 0;
  virtual std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir) = 0;
  virtual bool HasRelationship(RelDirection dir) = 0;
  virtual bool HasRelationship(RelDirection dir, std::string target) = 0;
  virtual bool HasRelationship(RelDirection dir, std::string first, std::string second) = 0;
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMECOLLEAGUE_H_
