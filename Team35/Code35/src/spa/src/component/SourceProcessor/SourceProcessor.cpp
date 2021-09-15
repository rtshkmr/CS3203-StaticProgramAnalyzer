#include <util/Logger.h>
#include <exception/SyntaxException.h>
#include <exception/IterationOneException.h>
#include "SourceProcessor.h"
#include "Parser.h"
#include "DesignExtractor.h"
#include "../PKB/PKB.h"

using namespace sp;

/**
 * Processes the file by parsing it and extracting the entities and relationships to be populated in the PKB.
 * Returns a newly created PKB.
 *
 * @param fileName The name of the file to be processed.
 * @return Source process status
 */
PKB* SourceProcessor::ProcessSourceFile(std::string file_name) {
  LOG(spa_logger << "=================== SOURCE PROCESSOR ===========================");
  LOG(spa_logger << "... processing source file");
  par::Parser parser;

  try {
    parser.Parse(file_name);
  } catch (SyntaxException s) {
    std::cerr << "Syntax Error\n";
    std::cerr << s.what() << std::endl;
    return new PKB();
  } catch (IterationOneException s) {
    std::cerr << "Syntax Error (due to Iteration 1 requirement)\n";
    std::cerr << s.what() << std::endl;
    return new PKB();
  }

  Deliverable* deliverable = parser.GetDeliverables();

  DesignExtractor design_extractor = DesignExtractor(deliverable);
  design_extractor.ExtractDesignAbstractions();

  PKB* new_pkb = new PKB();
  new_pkb->PopulateDataStructures(*deliverable);
  return new_pkb;
}
