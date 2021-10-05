#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_CALLSTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_CALLSTEXTRACTOR_H_

#include <datatype/Deliverable.h>

class CallsTExtractor {
 public:
  CallsTExtractor() = default;
  void Extract(Deliverable* deliverable);
 private:
  Deliverable* deliverable_;
  std::list<Procedure*>* ExtractCallsTFromProc(Procedure* calling_proc, std::vector<Procedure*>* visited_procedures);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_CALLSTEXTRACTOR_H_
