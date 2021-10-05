#include "CallsTExtractor.h"

/**
 * Extracts transitive Calls* relationship from Calls relationship in the deliverable.
 *
 * @param deliverable Deliverable populated by PSubsystem.
 * @throws someexception todo when there is cyclic call
 * pseudocode
 *
 */
void CallsTExtractor::Extract(Deliverable* deliverable) {
  this->deliverable_ = deliverable;

}
