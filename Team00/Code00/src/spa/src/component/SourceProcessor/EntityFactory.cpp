#include <cassert>
#include <stdexcept>
#include <iostream>
#include <utility>
#include "EntityFactory.h"
#include "model/Statement.h"

/**
 * Creates Entities out of the given tokens.
 * Must assume that the tokens are in the form of a statement e.g. the 1st word is the keyword
 * and that the tokenized statement is syntactically correct
 */
Entity* EntityFactory::CreateEntities(vector<Token> tokens) {
  Token first_token = tokens.front();
  const std::string keyword_token_string = first_token.GetTokenString();
  switch (first_token.GetTokenTag()) {
    case TokenTag::kProcedureKeyword: {
      assert(tokens[1].GetTokenTag() == TokenTag::kName);
      std::string proc_token_string = tokens[1].GetTokenString();
      return new Procedure(new ProcedureName(proc_token_string));
    }
    case TokenTag::kReadKeyword: {
      assert(tokens[1].GetTokenTag() == TokenTag::kName);
      std::string read_var_string = tokens[1].GetTokenString();
      return new ReadEntity(new Variable(new VariableName(read_var_string)));
    }
    case TokenTag::kPrintKeyword: {
      assert(tokens[1].GetTokenTag() == TokenTag::kName);
      std::string print_var_string = tokens[1].GetTokenString();
      return new PrintEntity(new Variable(new VariableName(print_var_string)));
    }
    case TokenTag::kCallKeyword: {
      assert(tokens[1].GetTokenTag() == TokenTag::kName);
      std::string call_proc_string = tokens[1].GetTokenString();
      return new CallEntity(new Procedure(new ProcedureName(call_proc_string)));
    }
    case TokenTag::kWhileKeyword: {
      return CreateConditionalEntity(tokens, TokenTag::kWhileKeyword);
    }
    case TokenTag::kIfKeyword: {
      return CreateConditionalEntity(tokens, TokenTag::kIfKeyword);
    }
    case TokenTag::kElseKeyword: {
      return new ElseEntity();
    }
    case TokenTag::kName: {   // assignment statement
      assert(tokens[1].GetTokenTag() == TokenTag::kAssignmentOperator);
      vector<Token>
          expression_tokens = GetExpressionTokens(tokens, TokenTag::kAssignmentOperator, TokenTag::kSemicolon);
      std::string expression_string = ConvertTokensToString(expression_tokens);
      vector<Variable*> expression_variables = GetVariablesFromExpressionTokens(expression_tokens);
      vector<ConstantValue*> expression_constants = GetConstantsFromExpressionTokens(expression_tokens);
      return new AssignEntity(
          new Variable(new VariableName(keyword_token_string)),
          expression_string,
          expression_variables,
          expression_constants);
    }
    default:throw std::invalid_argument("Tokens cannot be made into entity in EF.");
  }
}

/**
 * Creates an IfEntity or WhileEntity and extracts the string of the expression, and the expression's variables and
 * ConstantValues.
 *
 * @param tokens A tokenized statement.
 * @param entity_type Determines which conditional Entity is created.
 * @return IfEntity or WhileEntity.
 */
Entity* EntityFactory::CreateConditionalEntity(vector<Token> tokens, TokenTag entity_type) {
  assert(tokens[1].GetTokenTag() == TokenTag::kOpenBracket);

  vector<Token> expression_tokens = GetExpressionTokens(tokens, TokenTag::kOpenBracket, TokenTag::kCloseBracket);
  std::string expression_string = ConvertTokensToString(expression_tokens);
  vector<Variable*> expression_variables = GetVariablesFromExpressionTokens(expression_tokens);
  vector<ConstantValue*> expression_constants = GetConstantsFromExpressionTokens(expression_tokens);

  if (entity_type == TokenTag::kIfKeyword) {
    return new IfEntity(expression_string, expression_variables, expression_constants);
  } else if (entity_type == TokenTag::kWhileKeyword) {
    return new WhileEntity(expression_string, expression_variables, expression_constants);
  } else {
    throw std::invalid_argument("Entity type for creating Conditional Entity is wrong.");
  }
}

/**
 * Gets Tokens that make an expression,
 * from the vector of tokens passed in which is in the form of a whole tokenized statement.
 * The expression is starts after the Token with a TokenTag of startTag and ends before the Token with a TokenTag of
 * endTag.
 *
 * @param tokens Tokens that represent a statement.
 * @param startTag The TokenTag which indicates that the next token is the start of an expression.
 * @param endTag The TokenTag which indicates that the previous token is the end of an expression.
 * @return Vector of tokens which represents the expression alone.
 */
vector<Token> EntityFactory::GetExpressionTokens(vector<Token> tokens, TokenTag startTag, TokenTag endTag) {
  int iterator = -1;
  int tokens_size = tokens.size();
  // finding start tag
  for (int i = 0; i < tokens_size; ++i) {
    if (tokens[i].GetTokenTag() == startTag) {
      iterator = i + 1;
      break;
    }
  }
  if (iterator < 0) {
    throw std::invalid_argument("Tokens do not contain start-of-expression token when passed into EF.");
  }
  // collecting tokens while finding end tag
  vector<Token> expression_tokens;
  while (tokens[iterator].GetTokenTag() != endTag && iterator != tokens_size) {
    expression_tokens.push_back(tokens[iterator]);
    iterator++;
  }
  if (iterator > tokens_size) {
    throw std::out_of_range("Tokens do not contain end-of-expression token when passed into EF.");
  }
  return expression_tokens;
}

string EntityFactory::ConvertTokensToString(vector<Token> tokens) {
  std::string expression_string;
  for (auto &token : tokens) {
    expression_string += token.GetTokenString();
  }
  return expression_string;
}

vector<Variable*> EntityFactory::GetVariablesFromExpressionTokens(vector<Token> tokens) {
  vector<Variable*> variables;
  for (auto &token: tokens) {
    if (token.GetTokenTag() == TokenTag::kName) {
      variables.push_back(new Variable(new VariableName(token.GetTokenString())));
    }
  }
  return variables;
}

vector<ConstantValue*> EntityFactory::GetConstantsFromExpressionTokens(vector<Token> tokens) {
  vector<ConstantValue*> constants;
  for (auto &token: tokens) {
    if (token.GetTokenTag() == TokenTag::kInteger) {
      constants.push_back(new ConstantValue(token.GetTokenString()));
    }
  }
  return constants;
}
