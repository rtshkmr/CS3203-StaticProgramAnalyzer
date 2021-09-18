/**
 * This body file contains the implementation of AssignmentExpression functions.
 */

#include <regex>
#include <stack>
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
 * Check if the given character is an operator.
 * An operator character refers to ( ) + - * / % ^
 * @param c The character to check
 * @return true if it is one of the character; false otherwise.
 */
bool IsOperator(char &c) {
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == '(' || c == ')')
    return true;
  else
    return false;
}

// Operator Rank only returns the input value rank.
// lower rank = push
// always return an odd number so stack value is even.
int OperatorRank(char &c) {
  if (c == ')') return 7;
  if (c == '(') return -1;
  if (c == '+' || c == '-') return 1;
  if (c == '*' || c == '/' || c == '%') return 3;
  if (c == '^') return 5;
  return -INT32_MAX;
}

std::string ConvertToStringAndClearBuffer(std::vector<char>* chara) {
  std::string s(chara->begin(), chara->end());
  chara->clear();

  if (!s.empty()) {
    if (s.back() != ' ') {
      s.push_back(' '); /// IMPORTANT: separate variable 'a' and 'b' with 'ab'
    }
  }

  return s;
}

std::vector<std::string> ConvertEquationIntoTokens(std::string eqn) {
  std::vector<char> chara;
  std::vector<std::string> tokens;
  std::string space;
  space.push_back(' ');
  tokens.push_back(space); // Insert a space in front of variable -- so that CheckExist does not match partial var

  std::stack<char> operator_stack;

  for (char &c: eqn) {
    if (IsOperator(c)) {
      std::string s = ConvertToStringAndClearBuffer(&chara);
      if (!s.empty()) {
        tokens.push_back(s);
      }

      int rank = OperatorRank(c);
      if (rank == -1) {
        operator_stack.push(c);
        continue;
      }
      if (!operator_stack.empty()) {
        int stack_rank = OperatorRank(operator_stack.top()) + 1; //stack rank always higher, due to left-associativity

        if (rank == 7) { //close bracket --> force pop to stack's (
          int op_rank = 0;
          do {
            char op = operator_stack.top();
            operator_stack.pop();
            std::string s2;
            s2.push_back(op);
            s2.push_back(' '); // add a space for every operator push.

            op_rank = OperatorRank(op);
            if (op_rank != -1) {
              tokens.push_back(s2);
            } else {
              break;
            }
          } while (op_rank != -1);

        } else if (rank < stack_rank) {
          while (rank < stack_rank) {
            char op = operator_stack.top();
            operator_stack.pop();
            std::string s2;
            s2.push_back(op);
            s2.push_back(' ');
            tokens.push_back(s2);


            if (!operator_stack.empty())
              stack_rank = OperatorRank(operator_stack.top()) + 1;
            else break;
          }
          operator_stack.push(c);
        } else {
          operator_stack.push(c);
        }
      } else {
        operator_stack.push(c);
      }
    } else if (!(c == ' ' || c == '0')) {
      chara.push_back(c);
    } else if (c == '0') {
      if (!chara.empty()) {
        chara.push_back(c);
      }
    }
  }

  std::string s = ConvertToStringAndClearBuffer(&chara);
  if (!s.empty()) {
    tokens.push_back(s);
  }

  while (!operator_stack.empty()) {
    char op = operator_stack.top();
    operator_stack.pop();
    std::string s2;
    s2.push_back(op);
    tokens.push_back(s2);
    s2.push_back(' '); // add a space for every operator push.
  }

  return tokens;
}

/**
 * This function contains the algorithm to convert an infix assignment expression into postfix expression.
 * @param infix The infix assignment to be converted.
 * @return The assignment which is transformed into postfix style.
 */
std::string AssignmentExpression::ParseInfixToPostfix(std::string infix) {

  ///Assumption: equation has been checked, and in correct format.

  std::vector<std::string> tokens = ConvertEquationIntoTokens(infix);

  std::string combine;

  for (std::string s: tokens) {
    combine.append(s);
  }

  return combine;
}

std::string AssignmentExpression::GetExpressionString() {
  // start from 1 because the first chara is a space.
  if (expression_.back() == ' ') {
    return expression_.substr(1, expression_.size() - 2);
  } else {
    return expression_.substr(1, expression_.size() - 1);
  }
}

/**
 * This function will check if the expression contains the pattern given.
 * This is used in condition of _"infix"_
 * @param pattern The pattern to evaluate against this assignment expression.
 * @return true if exist; false otherwise.
 */
bool AssignmentExpression::CheckExist(std::string pattern) {
  std::string queryPostfix = ParseInfixToPostfix(pattern);

  if (expression_.find(queryPostfix) != std::string::npos) {
    return true;
  } else {
    return false;
  }
}

/**
 * This function will check if the expression is exactly the pattern given.
 * This is used in condition of "infix"
 * @param pattern The pattern to evaluate against this assignment expression.
 * @return true if exist; false otherwise.
 */
bool AssignmentExpression::CheckExact(std::string pattern) {
  std::string queryPostfix = ParseInfixToPostfix(pattern);

  return expression_ == queryPostfix;
}
