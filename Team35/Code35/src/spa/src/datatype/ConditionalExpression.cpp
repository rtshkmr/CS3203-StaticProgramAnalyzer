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
  var_list_ = SortVariable(var_list);
}

/**
 * This function contains the algorithm to sort a vector<Variable*>.
 * @param var_list The vector<Variable*> to be sorted.
 * @return The vector<Variable*> after sorting.
 */
std::vector<Variable*> ConditionalExpression::SortVariable(std::vector<Variable*> var_list) {
  std::vector<Variable*> var_list_copy = var_list;
  std::sort(var_list_copy.begin(), var_list_copy.end(),
            [](Variable* a, Variable* b) { return *a->GetName() < *b->GetName(); });
  return var_list_copy;
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
  int start = 0;
  int end = var_list_.size() - 1;

  while (start <= end) {
    int mid = ( start + end ) / 2;
    if (*(var_list_.at(mid)->GetName()) == *(var->GetName())) return true;
    if (*(var_list_.at(mid)->GetName()) < *(var->GetName())) {
      start = mid + 1;
    } else {
      end = mid - 1;
    }
  }
  return false;
}
