#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_VARIABLETEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_VARIABLETEXTRACTOR_H_

#include <datatype/Deliverable.h>

enum class VariableRel {
  kModifies,
  kUses
};

class VariableTExtractor {
 public:
  VariableTExtractor() = default;
  void Extract(Deliverable* deliverable, VariableRel rel_type);
 private:
  Deliverable* deliverable_{};
  VariableRel rel_type_{};
  std::unordered_map<Container*, std::list<Variable*>*>* rel_map_{};
  std::unordered_map<Variable*, std::list<Container*>*>* reverse_rel_map_{};

  void InitRelMaps();
  std::list<Variable*>* ExtractFromContainer(Container* container, std::vector<Procedure*>* extracted_procedures);
  void ExtractFromIfContainer(IfEntity* if_entity,
                              Container* container,
                              std::vector<Procedure*>* extracted_procedures);
  void ExtractFromWhileContainer(WhileEntity* while_entity,
                                 Container* container,
                                 std::vector<Procedure*>* extracted_procedures);
  void ExtractFromCallContainer(CallEntity* call_entity,
                                Container* container,
                                std::vector<Procedure*>* extracted_procedures);
  void EraseElse();
  void EraseReverseRelationship(Container* container, std::list<Variable*>* var_list);
  void AddRelationship(Container* container, std::list<Variable*>* var_list);
  void AddRelationship(Statement* statement, Variable* var);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_VARIABLETEXTRACTOR_H_
