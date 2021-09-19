#ifndef AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_USESEXTRACTOR_H_
#define AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_USESEXTRACTOR_H_

#include <datatype/Deliverable.h>

class UsesExtractor {
 public:
  UsesExtractor() = default;
  void Extract(Deliverable* deliverable);
 private:
  Deliverable* deliverable_;

  std::list<Variable*>* ExtractUsesInContainer(Container* container, std::vector<Procedure*>* extracted_procedures);
  void ExtractUsesInIfContainer(IfEntity* if_entity,
                                Container* container,
                                std::vector<Procedure*>* extracted_procedures);
  void ExtractUsesInWhileContainer(WhileEntity* while_entity,
                                   Container* container,
                                   std::vector<Procedure*>* extracted_procedures);
  void ExtractUsesInCallContainer(CallEntity* call_entity,
                                  Container* container,
                                  std::vector<Procedure*>* extracted_procedures);
  void EraseElseAndProcFromUses();
  void EraseReverseRelationship(Container* container, std::list<Variable*>* var_list);
};

#endif //AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_USESEXTRACTOR_H_
