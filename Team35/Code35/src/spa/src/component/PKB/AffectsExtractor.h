#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

class AffectsExtractor {
 public:
  std::vector<Entity*> GetAffects(int target);
  std::vector<Entity*> GetAffectedBy(int target);
  void Delete();
 private:
  std::unordered_map<Statement*, std::list<Statement*>*> affects_map_;
  std::unordered_map<Statement*, std::list<Statement*>*> affected_by_map_;
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSEXTRACTOR_H_