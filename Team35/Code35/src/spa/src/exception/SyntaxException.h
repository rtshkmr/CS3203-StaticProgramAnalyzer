/**
 * This exception is thrown when a syntax error was found during parsing.
 */

#ifndef AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_
#define AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_

#include <exception>
#include <stdexcept>
#include <util/Logger.h>


constexpr auto E = [](auto msg) {
  LOG
      (spa_logger << Logger::Prettify(msg));
};


/**
 * Base class for all SPA exceptions that happen at runtime.
 */
class SpaException : public std::runtime_error {
 public:
  SpaException(char const* const message) : std::runtime_error(message) {
  };

};

/**
 * TODO: derive Syntax and Semantic Exception classes from SpaException class
 */
class SyntaxException : public std::runtime_error {
 public:
  // todo: rename _Message (see ide warning about it being a reserved variable name)
  SyntaxException(char const* const _Message) : std::runtime_error(_Message) {
    std::string prefix = "[SYNTAX EXCEPTION RAISED]: ";
    E(prefix + _Message);
  };
};

class SemanticException : public SpaException {
  SemanticException(char const* const message) : SpaException(message) {
    std::string prefix = "[SEMANTIC EXCEPTION RAISED]: ";
    E(prefix + message);
  };
};

#endif //AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_
