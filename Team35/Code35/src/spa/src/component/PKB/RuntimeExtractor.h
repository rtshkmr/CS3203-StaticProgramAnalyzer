#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_

#include <model/Entity.h>
#include <component/QueryProcessor/types/Types.h>
#include "PKB.h"

/**
 * This class acts as a facade for runtime extraction of relationships, for the QueryProcessor to query.
 */
class RuntimeExtractor {
 private:
  PKB* pkb_;
 public:
  RuntimeExtractor(PKB* pkb);
  std::vector<Entity*> GetRelationships(RelRef ref, std::string target);
  bool HasAffects();
  bool HasAffectsT();
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
