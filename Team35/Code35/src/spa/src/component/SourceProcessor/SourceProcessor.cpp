#include <util/Logger.h>
#include "SourceProcessor.h"
#include "Parser.h"
#include "DesignExtractor.h"
#include "../PKB/PKB.h"

using namespace sp;

/**
 * Processes the file by parsing it and extracting the entities and relationships to be populated in the PKB.
 *
 * @param fileName The name of the file to be processed.
 * @return Source process status
 */
 // todo: [cosmetic] instead of returning void and having a side-effect, return pkb directly
void SourceProcessor::ProcessSourceFile(std::string file_name, PKB* pkb) {
  LOG(spa_logger << "=================== SOURCE PROCESSOR ===========================");
  LOG(spa_logger << "... processing source file");
  par::Parser parser;
  parser.Parse(file_name);
  Deliverable* deliverable = parser.GetDeliverables();
  DesignExtractor design_extractor = DesignExtractor(deliverable);
//  LOG(spa_logger << "...");
  design_extractor.ExtractDesignAbstractions();
  pkb->PopulateDataStructures(*deliverable);
}
