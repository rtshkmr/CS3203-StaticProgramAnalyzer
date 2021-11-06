#ifndef AUTOTESTER_SOURCEPROCESSOR_H
#define AUTOTESTER_SOURCEPROCESSOR_H

#include <string>
#include <datatype/Deliverable.h>
#include "../PKB/PKB.h"
#include "Parser.h"

/**
 * This namespace provides a scope for SourceProcessor related declarations. This only includes declarations on the
 * abstraction level that communicates between the Parser, DesignExtractor and PKB.
 */
namespace sp {
/**
 * This class manages the interactions between the Parser, DesignExtractor and the PKB. It preprocesses a source
 * file by calling the Parser and DesignExtractor and populates the PKB when the preprocessing is done.
 */
class SourceProcessor {

 public:
  SourceProcessor() = default;

  static PKB* ProcessSourceFile(const std::string& file_name);
  static void Terminate(const std::string &msg);
};
}

#endif //AUTOTESTER_SOURCEPROCESSOR_H
