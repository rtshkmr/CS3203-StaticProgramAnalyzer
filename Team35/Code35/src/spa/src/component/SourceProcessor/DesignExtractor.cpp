#include <component/SourceProcessor/Extractors/ParentTExtractor.h>
#include <component/SourceProcessor/Extractors/FollowsTExtractor.h>
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
  auto calls_t_extractor = TransitiveExtractor<Procedure>(deliverable_);
  calls_t_extractor.Extract(&deliverable_->calls_T_hash_, &deliverable_->calls_hash_, TransitiveRel::kCalls);

  auto variable_t_extractor = VariableTExtractor(deliverable_);
  variable_t_extractor.Extract(VariableRel::kUses);
  variable_t_extractor.Extract(VariableRel::kModifies);

  ParentTExtractor parent_t_extractor{};
  parent_t_extractor.Extract(deliverable_);

  FollowsTExtractor follows_t_extractor{};
  follows_t_extractor.Extract(deliverable_);

  NextExtractor next_extractor{};
  next_extractor.Extract(deliverable_);
}
