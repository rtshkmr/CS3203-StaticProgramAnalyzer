/**
 * This header file contains the object and method signatures of @code{ConditionalExpression}.
 */

#ifndef AUTOTESTER_CONDITIONALEXPRESSION_H
#define AUTOTESTER_CONDITIONALEXPRESSION_H

#include <string>
#include "model/Entity.h"

class ConditionalExpression {
 private:
  std::vector<Variable*> var_list_;

 public:
  ConditionalExpression(const std::string& expr, std::vector<Variable*> var_list);

  std::vector<Variable*> GetVariableList();

  bool CheckExist(Variable* var);
};

#endif //AUTOTESTER_CONDITIONALEXPRESSION_H
