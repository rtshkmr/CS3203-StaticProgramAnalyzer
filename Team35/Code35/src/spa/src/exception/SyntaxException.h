/**
 * This exception is thrown when a syntax error was found during parsing.
 */

#ifndef AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_
#define AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_

#include <exception>
#include <stdexcept>

/**
 * Base class for all SPA exceptions that happen at runtime.
 */
 class SpaException: public std::runtime_error {
  public:
   SpaException(char const* const message) : std::runtime_error(message) {};

 };

/**
 * TODO: derive Syntax and Semantic Exception classes from SpaException class
 */
class SyntaxException : public std::runtime_error {
 public:
  SyntaxException(char const* const _Message) : std::runtime_error(_Message) {};
};

#endif //AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_
