#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMEMEDIATOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMEMEDIATOR_H_

/**
 * Mediator interface to communicate with Colleague extractors.
 */
class RuntimeMediator {
 public:
  virtual std::vector<Entity*> GetRelationship(PKBRelRefs ref, std::string target) = 0;
  virtual std::vector<Entity*> GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de) = 0;
  virtual std::vector<std::tuple<Entity*, Entity*>> GetRelationshipByTypes(PKBRelRefs ref,
                                                                           DesignEntity first,
                                                                           DesignEntity second) = 0;
  virtual bool HasRelationship(PKBRelRefs ref) = 0;
  virtual bool HasRelationship(PKBRelRefs ref, std::string target) = 0;
  virtual bool HasRelationship(PKBRelRefs ref, std::string first, std::string second) = 0;
  virtual bool HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity second) = 0;
};
#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_PKB_EXTRACTOR_RUNTIMEMEDIATOR_H_
