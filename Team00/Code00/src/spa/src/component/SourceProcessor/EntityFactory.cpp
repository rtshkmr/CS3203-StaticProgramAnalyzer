#include <cassert>
#include <stdexcept>
#include <iostream>
#include "EntityFactory.h"
#include "model/Statement.h"

Entity* EntityFactory::CreateEntities(vector<Token> tokens) {
  /**
   * Must assume that the tokens are in the form of a statement i.e. the 1st word is the keyword
   * and that the tokenized statement is syntactically correct
   */
  Token keyword_token = tokens.front();
  const std::string keyword_token_string = keyword_token.GetTokenString();
  if (keyword_token_string == "procedure") {
    // assert(tokens[1].token_tag_ == TokenTag::kName);
    std::string token_string = tokens[1].GetTokenString();
    // todo: supposed to link statement list
    return new Procedure(new ProcedureName(token_string));
  } else if (keyword_token_string == "read") {
    std::string read_var_string = tokens[1].GetTokenString();
    // assert(tokens[1].token_tag_ == TokenTag::kName);
    return new ReadEntity(new Variable(new VariableName(read_var_string)));
  } else if (keyword_token_string == "print") {
    std::string print_var_string = tokens[1].GetTokenString();
    // assert(tokens[1].token_tag_ == TokenTag::kName);
    return new PrintEntity(new Variable(new VariableName(print_var_string)));
  } else if (keyword_token_string == "call") {
    std::string call_proc_string = tokens[1].GetTokenString();
    // assert(tokens[1].token_tag_ == TokenTag::kName);
    // todo: supposed to link an existing procedure with this call entity?
    return new CallEntity(new Procedure(new ProcedureName(call_proc_string)));
  } else if (keyword_token_string == "while") {
    std::string expression_string = GetExpression(tokens, "(", ")");
    // todo: supposed to link statement list to this while entity
    return new WhileEntity(expression_string);
  } else if (keyword_token_string == "if") {
    std::string expression_string = GetExpression(tokens, "(", ")");
    // todo: supposed to link statement list to this if entity
    return new IfEntity(expression_string);
  } else if (keyword_token_string == "else") {
    return new ElseEntity();
  } else if (keyword_token.token_tag_ == TokenTag::kName) {
    std::string expression_string = GetExpression(tokens, "=", ";");
    return new AssignEntity(new Variable(new VariableName(keyword_token_string)), expression_string);
  } else {
    throw std::invalid_argument("Tokens cannot be made into entity in EF.");
  }
}

std::string EntityFactory::GetExpression(vector<Token> tokens, const std::string &start, const std::string &end) {
  // assert(tokens[1].GetTokenString() == start);
  int iterator = 2;
  std::string expression_string;
  while (tokens[iterator].GetTokenString() != end && iterator != tokens.size()) {
    expression_string += tokens[iterator].GetTokenString();
    iterator++;
  }
  return expression_string;
}
