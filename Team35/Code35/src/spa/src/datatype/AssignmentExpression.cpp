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

// Operator Rank only returns the input value rank.
// lower rank = push
// always return an odd number so stack value is even.
// if it is not operator, return -INT32_MAX;
int OperatorRank(char& c) {
  switch (c) {
    case ')': return 7;
    case '^': return 5;
    case '*':
    case '/':
    case '%': return 3;
    case '+':
    case '-': return 1;
    case '(': return -1;
    default: return -INT32_MAX;
  }
}

/**
 * Convert the input vector<char> into a String, and clear vector<chara>.
 * Insert this string into vector<string>
 * @param chara The array of char to be converted into String.
 * @param tokens The vector to push the converted String.
 */
void ConvertToStringAndClearBuffer(std::vector<char>* chara, std::vector<std::string>* tokens) {
  std::string s(chara->begin(), chara->end());
  chara->clear();

  if (!s.empty()) {
    if (s.back() != ' ') {
      s.push_back(' '); /// IMPORTANT: separate variable 'a' and 'b' with 'ab'
    }
    tokens->push_back(s);
  }
}

void PushTopOperatorToTokenStack(std::stack<char>* operator_stack, std::vector<std::string>* tokens) {
  std::string s2 = std::string() + operator_stack->top() + ' ';
  operator_stack->pop();
  tokens->push_back(s2);
}

void HandleCloseBracket(std::stack<char>* operator_stack, std::vector<std::string>* tokens) {
  int op_rank = OperatorRank(operator_stack->top());
  while (op_rank != -1) {
    PushTopOperatorToTokenStack(operator_stack, tokens);
    op_rank = OperatorRank(operator_stack->top());
  }
  operator_stack->pop(); // this is a (used) open bracket.
}

std::vector<std::string> ConvertEquationIntoTokens(std::string eqn) {
  std::vector<char> chara;
  std::vector<std::string> tokens;
  std::stack<char> operator_stack;

  for (char &c: eqn) {
    if (OperatorRank(c) == -INT32_MAX) {
      if (c != ' ') {
        chara.push_back(c);
      }
      continue;
    }

    // Handle Operator type
    ConvertToStringAndClearBuffer(&chara, &tokens);

    int rank = OperatorRank(c);

    if (rank == -1 || operator_stack.empty()) { // ( found or no operator to pop --> Push and leave
      operator_stack.push(c);
      continue;
    }

    int stack_rank = OperatorRank(operator_stack.top()) + 1; //stack rank always higher, due to left-associativity

    if (rank == 7) { // ) found; Pop everything until Open Bracket, and leave.
      HandleCloseBracket(&operator_stack, &tokens);
      continue;
    }

    while (rank < stack_rank) { // Perform any bigger operators, then push this op to stack
      PushTopOperatorToTokenStack(&operator_stack, &tokens);
      stack_rank = (operator_stack.empty()) ? -INT32_MAX : OperatorRank(operator_stack.top()) + 1;
    }
    operator_stack.push(c);
  }

  ConvertToStringAndClearBuffer(&chara, &tokens);

  while (!operator_stack.empty()) {
    PushTopOperatorToTokenStack(&operator_stack, &tokens);
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
  std::string combine = " "; // Insert a space in front of variable -- so that CheckExist does not match partial var
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
