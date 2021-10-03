/**
 * This header file contains the object and method signatures of @code{ConditionalExpression}.
 */

#ifndef AUTOTESTER_CONDITIONALEXPRESSION_H
#define AUTOTESTER_CONDITIONALEXPRESSION_H

#include <string>
#include "model/Entity.h"

class ConditionalExpression {
 private:
  std::string actual_expression_; // TODO: check if it is required.
  std::vector<Variable*> var_list_;

  std::vector<Variable*> SortVariable(std::vector<Variable*> var_list);

 public:
  ConditionalExpression(std::string expr, std::vector<Variable*> var_list);

  std::string GetExpressionString();

  std::vector<Variable*> GetVariableList();

  bool CheckExist(Variable* var);
};

#endif //AUTOTESTER_CONDITIONALEXPRESSION_H
