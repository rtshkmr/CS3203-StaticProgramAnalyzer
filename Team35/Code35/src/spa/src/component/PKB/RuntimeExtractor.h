#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_

#include <model/Entity.h>
#include <component/QueryProcessor/types/Types.h>
#include <component/PKB/extractor/NextBipExtractor.h>
#include <component/PKB/extractor/NextBipTExtractor.h>
#include <component/PKB/extractor/RuntimeMediator.h>
#include "PKB.h"
#include "NextTExtractor.h"
#include "AffectsExtractor.h"
#include "AffectsTExtractor.h"

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

 private:
  PKB* pkb_;
  NextTExtractor next_t_extractor_ = NextTExtractor();
  AffectsExtractor affects_extractor_ = AffectsExtractor();
  AffectsTExtractor affects_t_extractor_ = AffectsTExtractor();
  NextBipExtractor next_bip_extractor_;
  NextBipTExtractor next_bip_t_extractor_;

  static inline std::unordered_set<PKBRelRefs> runtime_relationships = {
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

  std::vector<Entity*> GetNextT(int target);
  std::vector<Entity*> GetPrevT(int target);
  std::vector<Entity*> GetAffects(int target);
  std::vector<Entity*> GetAffectedBy(int target);
  std::vector<Entity*> GetAffectsT(int target);
  std::vector<Entity*> GetAffectedByT(int target);
  std::vector<Entity*> GetNextBip(std::string target);
  std::vector<Entity*> GetPrevBip(std::string target);
  std::vector<Entity*> GetNextBipT(std::string target);
  std::vector<Entity*> GetPrevBipT(std::string target);

  std::vector<Entity*> GetNextT(DesignEntity de);
  std::vector<Entity*> GetPrevT(DesignEntity de);
  std::vector<Entity*> GetAffects(DesignEntity de);
  std::vector<Entity*> GetAffectedBy(DesignEntity de);
  std::vector<Entity*> GetAffectsT(DesignEntity de);
  std::vector<Entity*> GetAffectedByT(DesignEntity de);
  std::vector<Entity*> GetNextBip(DesignEntity de);
  std::vector<Entity*> GetPrevBip(DesignEntity de);

  std::vector<std::tuple<Entity*, Entity*>> GetNextT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetPrevT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffects(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffectedBy(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffectsT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetAffectedByT(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetNextBip(DesignEntity first, DesignEntity second);
  std::vector<std::tuple<Entity*, Entity*>> GetPrevBip(DesignEntity first, DesignEntity second);

  bool HasAffects();
  bool HasAffectedBy();
  bool HasNextBip();
  bool HasPrevBip();

  bool HasAffects(int first);
  bool HasAffectedBy(int first);
  bool HasAffectsT(int first);
  bool HasAffectedByT(int first);
  bool HasNextBip(std::string first);
  bool HasPrevBip(std::string first);

  bool HasNextT(int first, int second);
  bool HasAffects(int first, int second);
  bool HasAffectedBy(int first, int second);
  bool HasAffectsT(int first, int second);
  bool HasAffectedByT(int first, int second);
  bool HasNextBip(std::string first, std::string second);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_RUNTIMEEXTRACTOR_H_
