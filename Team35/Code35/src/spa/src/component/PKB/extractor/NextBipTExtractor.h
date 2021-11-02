#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_

#include <component/PKB/PKB.h>
#include "RuntimeColleague.h"
#include "RuntimeMediator.h"

class NextBipTExtractor : public RuntimeColleague {
 public:
  NextBipTExtractor() = default;
  explicit NextBipTExtractor(PKB* pkb);
  std::vector<Entity*> GetRelationship(RelDirection dir, std::string target) override;
  std::vector<Entity*> GetFirstEntityOfRelationship(RelDirection dir) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(RelDirection dir) override;
  bool HasRelationship(RelDirection dir) override;
  bool HasRelationship(RelDirection dir, std::string target) override;
  bool HasRelationship(RelDirection dir, std::string first, std::string second) override;
  void SetMediator(RuntimeMediator* rtm);
 private:
  PKB* pkb_;
  RuntimeMediator* rtm_;
  void PopulateRelationships();
  PKBRelRefs GetPKBRelRef(RelDirection dir);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_NEXTBIPTEXTRACTOR_H_
