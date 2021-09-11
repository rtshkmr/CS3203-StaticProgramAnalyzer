#ifndef AUTOTESTER_DESIGNEXTRACTOR_H
#define AUTOTESTER_DESIGNEXTRACTOR_H

#include <datatype/Deliverable.h>

/**
 * This class encapsulates the extraction of design relationships from the data structures available in the Source
 * Processor.
 */
class DesignExtractor {
 public:
  DesignExtractor(Deliverable* deliverable);
  void ExtractDesignAbstractions();
  void ExtractUses();
  void ExtractModifies();
  void ExtractParentT(std::unordered_map<Statement*, std::list<Statement*>*> parent_hash);
  void ExtractFollowsT(std::unordered_map<Statement*, Statement*> follow_hash);
  void ExtractParentOfT(std::unordered_map<Statement*, Statement*> child_to_parent_hash);
  void ExtractFollowedByT(std::unordered_map<Statement*, Statement*> followed_by_hash);
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
};

#endif //AUTOTESTER_DESIGNEXTRACTOR_H
