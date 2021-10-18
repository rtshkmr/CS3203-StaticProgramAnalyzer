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
  explicit SpaException(char const* const message) : std::runtime_error(message) {
  };

};

/**
 * TODO: derive Syntax and Semantic Exception classes from SpaException class
 */
class SyntaxException : public SpaException {
 public:
  explicit SyntaxException(char const* const message) : SpaException(message) {
    std::string prefix = "[SYNTAX EXCEPTION RAISED]: ";
    E(prefix + message);
  };
};

class SemanticException : public SpaException {
 public:
  explicit SemanticException(char const* const message) : SpaException(message) {
    std::string prefix = "[SEMANTIC EXCEPTION RAISED]: ";
    E(prefix + message);
  };
};

//! Represents an error that occurs during parsing of PQL tokens into a Query
class PQLParseException : public SyntaxException {
 public:
  explicit PQLParseException(const std::string& msg = "") : SyntaxException(msg.c_str()) {}
};

//! Represents an error that occurs during the creation of PQL tokens
class PQLTokenizeException : public SyntaxException {
 public:
  explicit PQLTokenizeException(const std::string& msg = "") : SyntaxException(msg.c_str()) {}
};

//! Represents an error that occurs during the validation of a PQL query
class PQLValidationException : public SyntaxException {
 public:
  explicit PQLValidationException(const std::string& msg = "")
      : SyntaxException(msg.c_str()) {}
};

#endif //AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_
