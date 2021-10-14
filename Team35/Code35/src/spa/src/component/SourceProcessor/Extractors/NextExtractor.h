#ifndef AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_NEXTEXTRACTOR_H_
#define AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_NEXTEXTRACTOR_H_

#include <datatype/Deliverable.h>

class NextExtractor {
 public:
  NextExtractor() = default;
  void Extract(Deliverable* deliverable);
 private:
  Deliverable* deliverable_;
  std::vector<int>* visited_blocks_;

  void ExtractBlock(Block* block);
};

#endif //AUTOTESTER_CODE35_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_EXTRACTORS_NEXTEXTRACTOR_H_
