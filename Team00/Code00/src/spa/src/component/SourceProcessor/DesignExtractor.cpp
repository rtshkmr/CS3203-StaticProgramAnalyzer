#include "DesignExtractor.h"

/**
 * Extracts transitive relationships using the lists and tables from the deliverable.
 */
void DesignExtractor::ExtractDesignAbstractions(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

  ExtractUses();
  ExtractModifies();
  ExtractParentT(deliverable->parent_to_child_hash_);
  ExtractFollowsT(deliverable->follow_hash_);
  ExtractParentOfT(deliverable->child_to_parent_hash_);
  ExtractFollowedByT(deliverable->followed_by_hash_);
}

/**
 * Extracts transitive Uses relationship from nested containers and called procedures.
 * For e.g. there can be a while container contained in another while container.
 * A procedure can also call another procedure.
 */
void DesignExtractor::ExtractUses() {

}

/**
 * Extracts transitive Modifies relationship from nested containers and called procedures.
 * For e.g. there can be a while container contained in another while container.
 * A procedure can also call another procedure.
 */
void DesignExtractor::ExtractModifies() {

}

void DesignExtractor::ExtractParentT(std::unordered_map<Statement*, std::list<Statement*>*> parent_hash) {

}

void DesignExtractor::ExtractFollowsT(std::unordered_map<Statement*, Statement*> follow_hash) {

}

void DesignExtractor::ExtractParentOfT(std::unordered_map<Statement*, Statement*> child_to_parent_hash) {

}

void DesignExtractor::ExtractFollowedByT(std::unordered_map<Statement*, Statement*> followed_by_hash) {

}
