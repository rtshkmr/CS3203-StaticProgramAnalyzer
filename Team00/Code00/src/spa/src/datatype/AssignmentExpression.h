/**
 * This header file contains the object and method signatures of @code{AssignmentExpression}.
 */

#ifndef AUTOTESTER_ASSIGNMENTEXPRESSION_H
#define AUTOTESTER_ASSIGNMENTEXPRESSION_H

#include <string>

class AssignmentExpression {
 private:
  std::string expression_;

  std::string ParseInfixToPostfix(std::string infix);

 public:
  AssignmentExpression(std::string infix);

  bool CheckExist(std::string pattern);

  bool CheckExact(std::string pattern);

  bool CheckBefore(std::string pattern);

  bool CheckAfter(std::string pattern);
};

#endif //AUTOTESTER_ASSIGNMENTEXPRESSION_H
