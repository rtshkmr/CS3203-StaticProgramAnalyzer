/**
 * This header file contains the object and method signatures of @code{ConditionalExpression}.
 */

#ifndef AUTOTESTER_CONDITIONALEXPRESSION_H
#define AUTOTESTER_CONDITIONALEXPRESSION_H

#include <string>

class ConditionalExpression {
 private:
  std::string expression_;

  std::string ParseExpression(std::string inExpression);

 public:
  ConditionalExpression(std::string expr);

  std::string GetExpressionString();
};

#endif //AUTOTESTER_CONDITIONALEXPRESSION_H
