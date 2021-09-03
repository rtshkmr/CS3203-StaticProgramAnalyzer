/**
 * This body file contains the implementation of AssignmentExpression functions.
 */

#include "AssignmentExpression.h"

/**
 * This is the constructor for AssignmentExpression.
 * To create this object, you should be dealing with an assign-statement, and the input for this constructor
 *   is the exact expression found in SIMPLE program.
 * @param infix The infix assignment to be stored.
 */
AssignmentExpression::AssignmentExpression(std::string infix) {
  expression_ = ParseInfixToPostfix(infix);
}

/**
 * This function contains the algorithm to convert an infix assignment expression into postfix expression.
 * @param infix The infix assignment to be converted.
 * @return The assignment which is transformed into postfix style.
 */
std::string AssignmentExpression::ParseInfixToPostfix(std::string infix) {
  //TODO: change infix into postfix
  return infix;
}

/**
 * This function will check if the expression contains the pattern given.
 * This is used in condition of _"infix"_
 * @param pattern The pattern to evaluate against this assignment expression.
 * @return true if exist; false otherwise.
 */
bool AssignmentExpression::CheckExist(std::string pattern) {
  std::string queryPostfix = ParseInfixToPostfix(pattern);

  //TODO: check if expression CONTAINS queryPostfix

  return true;
}

/**
 * This function will check if the expression is exactly the pattern given.
 * This is used in condition of "infix"
 * @param pattern The pattern to evaluate against this assignment expression.
 * @return true if exist; false otherwise.
 */
bool AssignmentExpression::CheckExact(std::string pattern) {
  std::string queryPostfix = ParseInfixToPostfix(pattern);

  //TODO: check if expression == queryPostfix

  return true;
}

/**
 * This function will check if the expression is ends with the pattern given.
 * This is used in condition of _"infix"
 * @param pattern The pattern to evaluate against this assignment expression.
 * @return true if exist; false otherwise.
 */
bool AssignmentExpression::CheckBefore(std::string pattern) {
  std::string queryPostfix = ParseInfixToPostfix(pattern);

  //TODO: check if expression ENDS WITH pattern

  return true;
}

/**
 * This function will check if the expression is starts with the pattern given.
 * This is used in condition of "infix"_
 * @param pattern The pattern to evaluate against this assignment expression.
 * @return true if exist; false otherwise.
 */
bool AssignmentExpression::CheckAfter(std::string pattern) {
  std::string queryPostfix = ParseInfixToPostfix(pattern);

  //TODO: check if expression STARTS WITH pattern

  return true;
}
