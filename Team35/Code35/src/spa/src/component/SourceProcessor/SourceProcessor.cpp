#include <util/Logger.h>
#include <exception/SpaException.h>
#include "SourceProcessor.h"
#include "Parser.h"
#include "DesignExtractor.h"
#include "../PKB/PKB.h"

using namespace sp;

constexpr auto L = [](auto msg) {
  LOG
  (spa_logger << Logger::Prettify(msg));
};

/**
 * Processes the file by parsing it and extracting the entities and relationships to be populated in the PKB.
 * Returns a newly created PKB.
 *
 * @param fileName The name of the file to be processed.
 * @return Source process status
 */
PKB* SourceProcessor::ProcessSourceFile(const std::string& file_name) {
  L("[ENTER] SOURCE PROC ");
  L("... processing source file");
  par::Parser parser;
  Deliverable* deliverable;
  try {
    parser.Parse(file_name);
    deliverable = parser.GetDeliverables();
    auto design_extractor = DesignExtractor(deliverable);
    design_extractor.ExtractDesignAbstractions();
  } catch (SyntaxException s) {
    std::cerr << "Syntax Error\n" << s.what() << std::endl;
    Terminate(std::string("Unfortunately, there was a syntax error in the input SIMPLE Program:("));
  } catch (std::exception e) {
    std::cerr << "Exception error\n" << e.what() << std::endl;
    Terminate(std::string("Unfortunately, there was an unknown exception thrown due to an invalid SIMPLE program."));
  }

  PKB* new_pkb = new PKB();
  try {
    new_pkb->PopulateDataStructures(* deliverable);
  } catch (PKBException e) {
    std::cerr << "PKB error\n" << e.what() << std::endl;
    Terminate(std::string("Unfortunately, there was an unknown exception thrown due to an invalid SIMPLE program."));
  }
  return new_pkb;
}

/**
 * Terminates Parser execution and logger, and exits program.
 */
void SourceProcessor::Terminate(const std::string& msg) {
  std::string logger_output = msg + "\n [ERROR] TERMINATING PROGRAM";
  L(logger_output);
  LoggerTerminate();
  std::exit(EXIT_FAILURE);
}
