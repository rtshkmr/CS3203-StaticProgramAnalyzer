#include <util/Logger.h>
#include <exception/SyntaxException.h>
#include <exception/IterationOneException.h>
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
PKB* SourceProcessor::ProcessSourceFile(std::string file_name) {
  L("[ENTER] SOURCE PROC ");
  L("... processing source file");
  par::Parser parser;

  try {
    parser.Parse(file_name);
  } catch (SyntaxException s) {
    std::cerr << "Syntax Error\n";
    std::cerr << s.what() << std::endl;
    Terminate(std::string("Unfortunately, there was a syntax error in the input SIMPLE Program:("));
  } catch (IterationOneException s) {
    std::cerr << "Syntax Error (due to Iteration 1 requirement)\n";
    std::cerr << s.what() << std::endl;
    L("[EXIT] SOURCE PROC");
    Terminate(std::string("Unfortunately, the Source input had something that isn't supported for SPA Iteration 1"));
  } catch (std::exception e) {
    std::cerr << "Exception error\n";
    std::cerr << e.what() << std::endl;
    Terminate(std::string("Unfortunately, there was an unknown exception thrown due to an invalid SIMPLE program."));
  }

  Deliverable* deliverable = parser.GetDeliverables();
  DesignExtractor design_extractor = DesignExtractor(deliverable);
  design_extractor.ExtractDesignAbstractions();

  PKB* new_pkb = new PKB();
  new_pkb->PopulateDataStructures(* deliverable);
  return new_pkb;
}

/**
 * Terminates Parser execution and logger, and exits program.
 */
void SourceProcessor::Terminate(std::string msg) {
  std::string logger_output = msg + "\n [ERROR] TERMINATING PROGRAM";
  L(logger_output);
  LoggerTerminate();
  std::exit(EXIT_FAILURE);
}
