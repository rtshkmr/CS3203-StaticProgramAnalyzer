/**
 * This body file contains the implementation of ConditionalExpression functions.
 */

#include "ConditionalExpression.h"

/**
 * This is the constructor for ConditionalExpression.
 * To create this object, you should be dealing with an if-statement or while-statement,
 *   and the input for this constructor is the exact conditional expression found in SIMPLE program.
 * @param infix The conditional assignment to be stored.
 */
ConditionalExpression::ConditionalExpression(std::string expr) {
  expression_ = ParseExpression(expr);
}

/**
 * This function contains the algorithm to convert an conditional expression into <<SOME>> expression.
 * @param infix The conditional expression to be converted.
 * @return The expression which is transformed into <<SOME>> style.
 */
std::string ConditionalExpression::ParseExpression(std::string inExpression) {
  //TODO: logic for parsing expression.
  return inExpression;
}

std::string ConditionalExpression::GetExpressionString() {
  return expression_;
}
