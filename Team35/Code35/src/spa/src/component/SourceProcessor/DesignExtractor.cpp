#include <component/SourceProcessor/Extractors/NextExtractor.h>
#include <component/SourceProcessor/Extractors/TransitiveExtractor.h>
#include <component/SourceProcessor/Extractors/VariableTExtractor.h>
#include "DesignExtractor.h"

DesignExtractor::DesignExtractor(Deliverable* deliverable) {
  this->deliverable_ = deliverable;
}

/**
 * Extracts transitive relationships using the lists and tables from the deliverable,
 * namely Calls, Uses, Modifies, Parent* and Follow* and their reverse relationships.
 */
void DesignExtractor::ExtractDesignAbstractions() {
  auto procedure_extractor = TransitiveExtractor<Procedure>(deliverable_);
  procedure_extractor.Extract(deliverable_->GetCallsTMap(), deliverable_->GetCallsMap(), TransitiveRel::kCalls);

  auto statement_extractor = TransitiveExtractor<Statement>(deliverable_);
  statement_extractor.Extract(deliverable_->GetParentTMap(),
                              deliverable_->GetParentMap(),
                              TransitiveRel::kParent);
  statement_extractor.Extract(deliverable_->GetFollowsTMap(), deliverable_->GetFollowsMap(), TransitiveRel::kFollows);

  auto variable_t_extractor = VariableTExtractor(deliverable_);
  variable_t_extractor.Extract(VariableRel::kUses);
  variable_t_extractor.Extract(VariableRel::kModifies);

  NextExtractor next_extractor{};
  next_extractor.Extract(deliverable_);
}
