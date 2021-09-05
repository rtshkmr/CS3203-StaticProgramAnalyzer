#include <cassert>
#include <stdexcept>
#include <iostream>
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
  if (first_token.token_tag_ == TokenTag::kProcedureKeyword) {
    assert(tokens[1].token_tag_ == TokenTag::kName);
    std::string token_string = tokens[1].GetTokenString();
    return new Procedure(new ProcedureName(token_string));
  } else if (first_token.token_tag_ == TokenTag::kReadKeyword) {
    std::string read_var_string = tokens[1].GetTokenString();
    assert(tokens[1].token_tag_ == TokenTag::kName);
    return new ReadEntity(new Variable(new VariableName(read_var_string)));
  } else if (first_token.token_tag_ == TokenTag::kPrintKeyword) {
    std::string print_var_string = tokens[1].GetTokenString();
    assert(tokens[1].token_tag_ == TokenTag::kName);
    return new PrintEntity(new Variable(new VariableName(print_var_string)));
  } else if (first_token.token_tag_ == TokenTag::kCallKeyword) {
    std::string call_proc_string = tokens[1].GetTokenString();
    assert(tokens[1].token_tag_ == TokenTag::kName);
    return new CallEntity(new Procedure(new ProcedureName(call_proc_string)));
  } else if (first_token.token_tag_ == TokenTag::kWhileKeyword) {
    assert(tokens[1].token_tag_ == TokenTag::kOpenBracket);
    vector<Token> expression_tokens = GetExpressionTokens(tokens, TokenTag::kOpenBracket, TokenTag::kCloseBracket);
    std::string expression_string = ConvertTokensToString(expression_tokens);
    vector<Variable*> expression_variables = GetVariablesFromExpressionTokens(expression_tokens);
    vector<ConstantValue*> expression_constants = GetConstantsFromExpressionTokens(expression_tokens);
    return new WhileEntity(expression_string, expression_variables, expression_constants);
  } else if (first_token.token_tag_ == TokenTag::kIfKeyword) {
    assert(tokens[1].token_tag_ == TokenTag::kOpenBracket);
    vector<Token> expression_tokens = GetExpressionTokens(tokens, TokenTag::kOpenBracket, TokenTag::kCloseBracket);
    std::string expression_string = ConvertTokensToString(expression_tokens);
    vector<Variable*> expression_variables = GetVariablesFromExpressionTokens(expression_tokens);
    vector<ConstantValue*> expression_constants = GetConstantsFromExpressionTokens(expression_tokens);
    return new IfEntity(expression_string, expression_variables, expression_constants);
  } else if (first_token.token_tag_ == TokenTag::kElseKeyword) {
    return new ElseEntity();
  } else if (first_token.token_tag_ == TokenTag::kName) {
    assert(tokens[1].token_tag_ == TokenTag::kAssignmentOperator);
    vector<Token> expression_tokens = GetExpressionTokens(tokens, TokenTag::kAssignmentOperator, TokenTag::kSemicolon);
    std::string expression_string = ConvertTokensToString(expression_tokens);
    vector<Variable*> expression_variables = GetVariablesFromExpressionTokens(expression_tokens);
    vector<ConstantValue*> expression_constants = GetConstantsFromExpressionTokens(expression_tokens);
    return new AssignEntity(
        new Variable(new VariableName(keyword_token_string)),
        expression_string,
        expression_variables,
        expression_constants);
  } else {
    throw std::invalid_argument("Tokens cannot be made into entity in EF.");
  }
}

vector<Token> EntityFactory::GetExpressionTokens(vector<Token> tokens, TokenTag startTag, TokenTag endTag) {
  assert(tokens[1].token_tag_ == startTag);
  int iterator = 2;
  vector<Token> expression_tokens;
  int tokens_size = tokens.size();
  while (tokens[iterator].token_tag_ != endTag && iterator != tokens_size) {
    expression_tokens.push_back(tokens[iterator]);
    iterator++;
    if (iterator > tokens_size) {
      throw std::out_of_range("Tokens do not contain end-of-expression token when passed into EF.");
    }
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
    if (token.token_tag_ == TokenTag::kName) {
      variables.push_back(new Variable(new VariableName(token.GetTokenString())));
    }
  }
  return variables;
}

vector<ConstantValue*> EntityFactory::GetConstantsFromExpressionTokens(vector<Token> tokens) {
  vector<ConstantValue*> constants;
  for (auto &token: tokens) {
    if (token.token_tag_ == TokenTag::kInteger) {
      constants.push_back(new ConstantValue(token.GetTokenString()));
    }
  }
  return constants;
}
