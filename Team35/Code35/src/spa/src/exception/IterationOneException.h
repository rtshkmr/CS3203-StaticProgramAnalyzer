/**
 * This exception class is thrown when the system encounters an error with regards to Iteration 1 requirements,
 *   which may be used in Advance SPA.
 */

#ifndef AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_ITERATIONONEEXCEPTION_H_
#define AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_ITERATIONONEEXCEPTION_H_

#include <exception>
#include <stdexcept>

class IterationOneException : public std::runtime_error {
 public:
  IterationOneException(char const* const _Message) : std::runtime_error(_Message) {};
};

#endif //AUTOTESTER_SRC_UNIT_TESTING_SRC_EXCEPTION_ITERATIONONEEXCEPTION_H_
