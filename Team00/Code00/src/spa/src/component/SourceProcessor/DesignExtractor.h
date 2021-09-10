#ifndef AUTOTESTER_DESIGNEXTRACTOR_H
#define AUTOTESTER_DESIGNEXTRACTOR_H

#include <datatype/Deliverable.h>

/**
 * This class encapsulates the extraction of design relationships from the data structures available in the Source
 * Processor.
 */
class DesignExtractor {
 public:
  DesignExtractor() = default;
  void ExtractDesignAbstractions(Deliverable* deliverable);
 private:
  Deliverable* deliverable_;

  void ExtractUses(); // todo: add arguments when needed
  void ExtractModifies(); // todo: add arguments when needed
  void ExtractParentT(std::unordered_map<Statement*, std::list<Statement*>*> parent_hash);
  void ExtractFollowsT(std::unordered_map<Statement*, Statement*> follow_hash);
  void ExtractParentOfT(std::unordered_map<Statement*, Statement*> child_to_parent_hash);
  void ExtractFollowedByT(std::unordered_map<Statement*, Statement*> followed_by_hash);
};

#endif //AUTOTESTER_DESIGNEXTRACTOR_H
