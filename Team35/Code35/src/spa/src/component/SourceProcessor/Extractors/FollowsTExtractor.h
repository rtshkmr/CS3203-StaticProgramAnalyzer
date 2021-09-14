#ifndef AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_FOLLOWSTEXTRACTOR_H_
#define AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_FOLLOWSTEXTRACTOR_H_

#include <datatype/Deliverable.h>
class FollowsTExtractor {
 public:
  FollowsTExtractor() = default;
  void Extract(Deliverable* deliverable);
 private:
  Deliverable* deliverable_;

  std::list<Statement*>* ExtractFollowsTFromThread(Statement* top);
};

#endif //AUTOTESTER_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_FOLLOWSTEXTRACTOR_H_
