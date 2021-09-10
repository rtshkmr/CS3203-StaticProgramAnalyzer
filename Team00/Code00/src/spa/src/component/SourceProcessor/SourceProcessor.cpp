#include "SourceProcessor.h"
#include "Parser.h"
#include "DesignExtractor.h"

using namespace sp;

/**
 * Processes the file by parsing it and extracting the entities and relationships to be populated in the PKB.
 *
 * @param fileName The name of the file to be processed.
 * @return Source process status
 */
void SourceProcessor::ProcessSourceFile(std::string file_name) {
  par::Parser parser;
  parser.Parse(file_name);
  Deliverable* deliverable = parser.GetDeliverables();
  DesignExtractor design_extractor;
  design_extractor.ExtractDesignAbstractions(deliverable);
  // pkb.PopulateDataStructures();
}
