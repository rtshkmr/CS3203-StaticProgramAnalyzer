#include <cassert>
#include <stdexcept>
#include "EntityFactory.h"
#include "model/Statement.h"

Entity EntityFactory::CreateEntities(vector<Token> &tokens) {
  /**
   * Must assume that the tokens are in the form of a statement i.e. the 1st word is the keyword
   * and that the tokenized statement is syntactically correct
   */
  Token keyword_token = tokens.front();
  const std::string* keyword_token_string = keyword_token.GetTokenString();
  if (*keyword_token_string == string("procedure")) {
    std::string* token_string = tokens[1].GetTokenString();
    assert(tokens[1].token_tag_ == TokenTag::kName);
    ProcedureName procedure_name = ProcedureName(*token_string);
    // todo: supposed to link statement list
    return Procedure(&procedure_name);
  } else if (*keyword_token_string == string("read")) {// statements
    std::string* read_var_string = tokens[1].GetTokenString();
    assert(tokens[1].token_tag_ == TokenTag::kName);
    VariableName variable_name = VariableName(*read_var_string);
    Variable variable = Variable(&variable_name);
    return ReadEntity(&variable);
  } else if (*keyword_token_string == string("print")) {
    std::string* print_var_string = tokens[1].GetTokenString();
    assert(tokens[1].token_tag_ == TokenTag::kName);
    VariableName variable_name = VariableName(*print_var_string);
    Variable variable = Variable(&variable_name);
    return PrintEntity(&variable);
  } else if (*keyword_token_string == string("call")) {
    std::string* call_proc_string = tokens[1].GetTokenString();
    assert(tokens[1].token_tag_ == TokenTag::kName);
    ProcedureName procedure_name = ProcedureName(*call_proc_string);
    Procedure procedure = Procedure(&procedure_name);
    // todo: supposed to link an existing procedure with this call entity?
    return CallEntity(&procedure);
  } else if (*keyword_token_string == string("while")) {
    std::string expression_string = GetExpression(tokens, "(", ")");
    // todo: supposed to link statement list to this while entity
    return WhileEntity(expression_string);
  } else if (*keyword_token_string == string("if")) {
    std::string expression_string = GetExpression(tokens, "(", ")");
    // todo: supposed to link statement list to this if entity
    return IfEntity(expression_string);
  } else if (*keyword_token_string == string("else")) {
    return ElseEntity();
  } else if (keyword_token.token_tag_ == TokenTag::kName) {
    VariableName variable_name = VariableName(*keyword_token.GetTokenString());
    Variable variable = Variable(&variable_name);
    std::string expression_string = GetExpression(tokens, "=", ";");
    return AssignEntity(&variable, expression_string);
  } else {
    throw std::logic_error("Tokens cannot be made into entity in EF.");
  }
}

std::string EntityFactory::GetExpression(vector<Token> &tokens, std::string start, std::string end) {
  assert(*tokens[1].GetTokenString() == start);
  int iterator = 2;
  std::string expression_string;
  while (*tokens[iterator].GetTokenString() != end) {
    expression_string += *tokens[iterator].GetTokenString();
    iterator++;
  }
  return expression_string;
}
