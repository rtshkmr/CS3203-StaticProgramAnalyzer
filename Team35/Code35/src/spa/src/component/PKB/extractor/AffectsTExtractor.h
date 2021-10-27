#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_

#include <model/Entity.h>
#include <unordered_map>

class AffectsTExtractor {
 public:
  std::vector<Entity*> GetAffectsT(int target);
  std::vector<Entity*> GetAffectedByT(int target);
  void Delete();
 private:
  std::unordered_map<Statement*, std::list<Statement*>*> affects_t_map_;
  std::unordered_map<Statement*, std::list<Statement*>*> affected_by_t_map_;
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_AFFECTSTEXTRACTOR_H_
