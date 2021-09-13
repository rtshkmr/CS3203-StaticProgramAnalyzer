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

  std::string GetExpressionString();

  bool CheckExist(std::string pattern);

  bool CheckExact(std::string pattern);
};

#endif //AUTOTESTER_ASSIGNMENTEXPRESSION_H
