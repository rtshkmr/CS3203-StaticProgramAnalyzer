/**
 * This body file contains the implementation of ConditionalExpression functions.
 */

#include <model/Entity.h>
#include "ConditionalExpression.h"

/**
 * This is the constructor for ConditionalExpression.
 * To create this object, you should be dealing with an if-statement or while-statement,
 *   and the input for this constructor is the exact conditional expression found in SIMPLE program.
 * @param expr The actual expression found in source file.
 * @param var_list The variables within the actual expression.
 */
ConditionalExpression::ConditionalExpression(std::string expr, std::vector<Variable*> var_list) {
  actual_expression_ = expr;
  var_list_ = Variable::SortVariableVector(var_list);
}

/**
 * Gets the actual expression string as found in SIMPLE source file.
 * @return actual expression string.
 */
std::string ConditionalExpression::GetExpressionString() {
  return actual_expression_;
}

/**
 * Gets all the variable that exist in this condition expression.
 * @return A vector of Variable* that is in sorted order.
 */
std::vector<Variable*> ConditionalExpression::GetVariableList() {
  std::vector<Variable*> var_list_copy = var_list_;
  return var_list_copy;
}

/**
 * Performs a Binary Search to search if the input var is found within this.var_list_
 * This function should be used in (v,_,_) or ("v",_,_)
 * @param var Variable to be checked.
 * @return true if found; false otherwise.
 */
bool ConditionalExpression::CheckExist(Variable* var) {
  return std::binary_search(var_list_.begin(), var_list_.end(), var,
                            [](Variable* a, Variable* b) { return *a->GetName() < *b->GetName(); });
}
