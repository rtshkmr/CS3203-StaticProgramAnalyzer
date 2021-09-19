#ifndef AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_MODIFIESEXTRACTOR_H_
#define AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_MODIFIESEXTRACTOR_H_

#include <datatype/Deliverable.h>

class ModifiesExtractor {
 public:
  ModifiesExtractor() = default;
  void Extract(Deliverable* deliverable);
 private:
  Deliverable* deliverable_;

  std::list<Variable*>* ExtractModifiesInContainer(Container* container, std::vector<Procedure*>* extracted_procedures);
  void ExtractModifiesInIfContainer(IfEntity* if_entity,
                                    Container* container,
                                    std::vector<Procedure*>* extracted_procedures);
  void ExtractModifiesInWhileContainer(WhileEntity* while_entity,
                                       Container* container,
                                       std::vector<Procedure*>* extracted_procedures);
  void ExtractModifiesInCallContainer(CallEntity* call_entity,
                                      Container* container,
                                      std::vector<Procedure*>* extracted_procedures);
  void EraseElseAndProcFromModifies();
  void EraseReverseRelationship(Container* container, std::list<Variable*>* var_list);
};

#endif //AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_MODIFIESEXTRACTOR_H_
