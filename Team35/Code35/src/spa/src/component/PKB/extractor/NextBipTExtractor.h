#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_

#include <component/PKB/PKB.h>
#include "RuntimeColleague.h"
#include "RuntimeMediator.h"

class NextBipTExtractor : public RuntimeColleague {
 public:
  NextBipTExtractor() = default;
  explicit NextBipTExtractor(PKB* pkb);
  std::vector<Entity*> GetRelationship(RelDirection dir, int target) override;
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir) override;
  bool HasRelationship(RelDirection dir) override;
  bool HasRelationship(RelDirection dir, int target) override;
  bool HasRelationship(RelDirection dir, int first, int second) override;
  void SetMediator(RuntimeMediator* rtm);
 private:
  PKB* pkb_{};
  RuntimeMediator* rtm_{};
  std::unordered_map<Entity*, std::list<Entity*>*> t_map_;
  std::unordered_map<Entity*, std::list<Entity*>*> reverse_t_map_;
  std::unordered_map<Entity*, std::list<Entity*>*> non_t_map_;

  void PopulateRelationships();
  static PKBRelRefs GetPKBRelRef(RelDirection dir);
  void PopulateNextBipT();
  void ExtractRelationships(Entity* first_arg,
                            Entity* second_arg,
                            std::vector<Entity*>* visited_nodes);
  void AddRelationship(Entity* key, Entity* value);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_
