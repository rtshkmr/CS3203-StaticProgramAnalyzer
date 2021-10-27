#include <component/SourceProcessor/Extractors/UsesExtractor.h>
#include <component/SourceProcessor/Extractors/ModifiesExtractor.h>
#include <component/SourceProcessor/Extractors/ParentTExtractor.h>
#include <component/SourceProcessor/Extractors/FollowsTExtractor.h>
#include <component/SourceProcessor/Extractors/NextExtractor.h>
#include <component/SourceProcessor/Extractors/CallsTExtractor.h>
#include <component/SourceProcessor/Extractors/TransitiveExtractor.h>
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

  UsesExtractor uses_extractor{};
  uses_extractor.Extract(deliverable_);

  ModifiesExtractor modifies_extractor{};
  modifies_extractor.Extract(deliverable_);

  ParentTExtractor parent_t_extractor{};
  parent_t_extractor.Extract(deliverable_);

  FollowsTExtractor follows_t_extractor{};
  follows_t_extractor.Extract(deliverable_);

  NextExtractor next_extractor{};
  next_extractor.Extract(deliverable_);
}
