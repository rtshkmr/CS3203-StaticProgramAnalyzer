#ifndef AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_PARENTTEXTRACTOR_H_
#define AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_PARENTTEXTRACTOR_H_

#include <datatype/Deliverable.h>

class ParentTExtractor {
 public:
  ParentTExtractor() = default;
  void Extract(Deliverable* deliverable);
 private:
  Deliverable* deliverable_;

  std::list<Statement*>* ExtractChildrenTFromParent(Statement* parent);
};

#endif //AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_PARENTTEXTRACTOR_H_
