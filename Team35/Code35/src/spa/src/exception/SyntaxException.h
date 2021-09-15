/**
 * This exception is thrown when a syntax error was found during parsing.
 */

#ifndef AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_
#define AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_

#include <exception>
#include <stdexcept>

class SyntaxException : public std::runtime_error {
 public:
  SyntaxException(char const* const _Message) : std::runtime_error(_Message) {};
};

#endif //AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_SYNTAXEXCEPTION_H_
