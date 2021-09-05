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
  if (first_token.token_tag_ == TokenTag::kKeyword) {
    if (keyword_token_string == "procedure") {
      assert(tokens[1].token_tag_ == TokenTag::kName);
      std::string token_string = tokens[1].GetTokenString();
      // todo: supposed to link statement list, to do in psub
      return new Procedure(new ProcedureName(token_string));
    } else if (keyword_token_string == "read") {
      std::string read_var_string = tokens[1].GetTokenString();
      assert(tokens[1].token_tag_ == TokenTag::kName);
      return new ReadEntity(new Variable(new VariableName(read_var_string)));
    } else if (keyword_token_string == "print") {
      std::string print_var_string = tokens[1].GetTokenString();
      assert(tokens[1].token_tag_ == TokenTag::kName);
      return new PrintEntity(new Variable(new VariableName(print_var_string)));
    } else if (keyword_token_string == "call") {
      std::string call_proc_string = tokens[1].GetTokenString();
      assert(tokens[1].token_tag_ == TokenTag::kName);
      // todo: supposed to link an existing procedure with this call entity?
      return new CallEntity(new Procedure(new ProcedureName(call_proc_string)));
    } else if (keyword_token_string == "while") {
      assert(tokens[1].token_tag_ == TokenTag::kLeftBracket);
      std::string expression_string = GetExpression(tokens, "(", ")");
      // todo: supposed to link statement list to this while entity, to do in psub
      return new WhileEntity(expression_string);
    } else if (keyword_token_string == "if") {
      assert(tokens[1].token_tag_ == TokenTag::kLeftBracket);
      std::string expression_string = GetExpression(tokens, "(", ")");
      // todo: supposed to link statement list to this if entity, to do in psub
      return new IfEntity(expression_string);
    } else if (keyword_token_string == "else") {
      return new ElseEntity();
    } else {
      throw std::invalid_argument("Invalid keyword token passed into EF.");
    }
  } else if (first_token.token_tag_ == TokenTag::kName) {
    assert(tokens[1].GetTokenString() == "=");
    std::string expression_string = GetExpression(tokens, "=", ";");
    return new AssignEntity(new Variable(new VariableName(keyword_token_string)), expression_string);
  } else {
    throw std::invalid_argument("Tokens cannot be made into entity in EF.");
  }
}

std::string EntityFactory::GetExpression(vector<Token> tokens, const std::string &start, const std::string &end) {
  assert(tokens[1].GetTokenString() == start);
  int iterator = 2;
  std::string expression_string;
  int tokens_size = tokens.size();
  while (tokens[iterator].GetTokenString() != end && iterator != tokens_size) {
    expression_string += tokens[iterator].GetTokenString();
    iterator++;
    if (iterator > tokens_size) {
      throw std::out_of_range("Tokens do not contain end-of-expression token when passed into EF.");
    }
  }
  return expression_string;
}
