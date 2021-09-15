#include <exception/SyntaxException.h>
#include <exception/IterationOneException.h>
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
void SourceProcessor::ProcessSourceFile(std::string file_name) {
  par::Parser parser;
  try {
    parser.Parse(file_name);
  } catch (SyntaxException s) {
    std::cerr << "Syntax Error\n";
    std::cerr << s.what() << std::endl;
    return;
  } catch (IterationOneException s) {
    std::cerr << "Syntax Error (due to Iteration 1 requirement)\n";
    std::cerr << s.what() << std::endl;
    return;
  }

  Deliverable* deliverable = parser.GetDeliverables();
  DesignExtractor design_extractor = DesignExtractor(deliverable);
  design_extractor.ExtractDesignAbstractions();
  PKB pkb = PKB();
  pkb.PopulateDataStructures(*deliverable);

}
