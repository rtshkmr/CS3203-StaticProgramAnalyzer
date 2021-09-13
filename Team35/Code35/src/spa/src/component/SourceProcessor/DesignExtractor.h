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
 private:
  Deliverable* deliverable_;
};

#endif //AUTOTESTER_DESIGNEXTRACTOR_H
