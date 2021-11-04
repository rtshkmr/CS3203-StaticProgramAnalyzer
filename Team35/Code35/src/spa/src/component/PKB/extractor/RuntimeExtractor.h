#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_

#include <model/Entity.h>
#include <component/QueryProcessor/types/Types.h>
#include <component/PKB/extractor/NextBipExtractor.h>
#include <component/PKB/extractor/NextBipTExtractor.h>
#include <component/PKB/extractor/RuntimeMediator.h>
#include "component/PKB/PKB.h"
#include "NextTExtractor.h"
#include "AffectsExtractor.h"
#include "AffectsTExtractor.h"

const std::unordered_set<PKBRelRefs> runtime_relationships = {
    PKBRelRefs::kNextT,
    PKBRelRefs::kPreviousT,
    PKBRelRefs::kAffects,
    PKBRelRefs::kAffectedBy,
    PKBRelRefs::kAffectsT,
    PKBRelRefs::kAffectedByT,
    PKBRelRefs::kNextBip,
    PKBRelRefs::kPrevBip,
    PKBRelRefs::kNextBipT,
    PKBRelRefs::kPrevBipT
};

const std::unordered_set<DesignEntity> next_design_entities = {
    DesignEntity::kRead,
    DesignEntity::kPrint,
    DesignEntity::kCall,
    DesignEntity::kWhile,
    DesignEntity::kIf,
    DesignEntity::kAssign,
    DesignEntity::kStmt,
    DesignEntity::kProgLine
};

/**
 * This class acts as a facade for runtime extraction of relationships, for the DBManager to query. It abstracts out
 * the separate classes responsible for specific relationship extraction.
 */
class RuntimeExtractor : public RuntimeMediator {
 public:
  explicit RuntimeExtractor(PKB* pkb);
  std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string target) override;
  std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de) override;
  std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs ref, DesignEntity first, DesignEntity second) override;
  bool HasRelationship(PKBRelRefs ref) override;
  bool HasRelationship(PKBRelRefs ref, std::string first) override;
  bool HasRelationship(PKBRelRefs ref, std::string first, std::string second) override;
  bool HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) override;

  void Delete();

  static bool IsRuntimeRelationship(PKBRelRefs ref);

  std::vector<Entity*> GetScopedFirstEntities(PKBRelRefs ref, std::vector<Entity*> scoped_entities);
  std::vector<std::tuple<Entity*, Entity*>> GetAllRelationshipsScoped(PKBRelRefs ref,
                                                                      std::vector<Entity*> left_entities,
                                                                      std::vector<Entity*> right_entities);
 private:
  PKB* pkb_;
  NextTExtractor next_t_extractor_;
  AffectsExtractor affects_extractor_ = AffectsExtractor();
  AffectsTExtractor affects_t_extractor_;
  NextBipExtractor next_bip_extractor_;
  NextBipTExtractor next_bip_t_extractor_;

  std::vector<Entity*> GetNextT(int target);
  std::vector<Entity*> GetPrevT(int target);
  std::vector<Entity*> GetAffects(int target);
  std::vector<Entity*> GetAffectedBy(int target);
  std::vector<Entity*> GetAffectsT(int target);
  std::vector<Entity*> GetAffectedByT(int target);
  std::vector<Entity*> GetNextBip(int target);
  std::vector<Entity*> GetPrevBip(int target);
  std::vector<Entity*> GetNextBipT(int target);
  std::vector<Entity*> GetPrevBipT(int target);

  std::vector<Entity*> GetNextT(DesignEntity de);
  std::vector<Entity*> GetPrevT(DesignEntity de);
  std::vector<Entity*> GetAffects(DesignEntity de);
  std::vector<Entity*> GetAffectedBy(DesignEntity de);
  std::vector<Entity*> GetAffectsT(DesignEntity de);
  std::vector<Entity*> GetAffectedByT(DesignEntity de);
  std::vector<Entity*> GetNextBip(DesignEntity de);
  std::vector<Entity*> GetPrevBip(DesignEntity de);
  std::vector<Entity*> GetNextBipT(DesignEntity de);
  std::vector<Entity*> GetPrevBipT(DesignEntity de);

  std::vector<std::tuple<Entity*, Entity*>> GetNextT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetPrevT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffects(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffectedBy(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffectsT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffectedByT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetNextBip(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetPrevBip(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetNextBipT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetPrevBipT(DesignEntity first, DesignEntity second);

  bool HasAffects();
  bool HasAffectedBy();
  bool HasAffectsT();
  bool HasAffectedByT();
  bool HasNextBip();

  bool HasAffects(int first);
  bool HasAffectedBy(int first);
  bool HasAffectsT(int first);
  bool HasAffectedByT(int first);
  bool HasNextBip(int first);
  bool HasPrevBip(int first);

  bool HasNextT(int first, int second);
  bool HasAffects(int first, int second);
  bool HasAffectedBy(int first, int second);
  bool HasAffectsT(int first, int second);
  bool HasAffectedByT(int first, int second);
  bool HasNextBip(int first, int second);
  bool HasNextBipT(int first, int second);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
