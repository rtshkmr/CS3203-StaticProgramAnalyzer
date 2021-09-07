#include <cassert>
#include <stdexcept>
#include <iostream>
#include <utility>
#include "EntityFactory.h"
#include "model/Statement.h"

EntityFactory::EntityFactory(std::list<Procedure*>* proc_list,
                             std::list<Variable*>* var_list,
                             std::list<ConstantValue*>* const_list) {
  proc_list_ = proc_list;
  var_list_ = var_list;
  const_list_ = const_list;
}

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
      return CreateProcedure(proc_token_string);
    }
    case TokenTag::kReadKeyword: {
      assert(tokens[1].GetTokenTag() == TokenTag::kName);
      std::string read_var_string = tokens[1].GetTokenString();
      return new ReadEntity(RetrieveVariable(read_var_string));
    }
    case TokenTag::kPrintKeyword: {
      assert(tokens[1].GetTokenTag() == TokenTag::kName);
      std::string print_var_string = tokens[1].GetTokenString();
      return new PrintEntity(RetrieveVariable(print_var_string));
    }
    case TokenTag::kCallKeyword: {
      assert(tokens[1].GetTokenTag() == TokenTag::kName);
      std::string call_proc_string = tokens[1].GetTokenString();
      return new CallEntity(RetrieveProcedure(call_proc_string));
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
          RetrieveVariable(keyword_token_string),
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
  for (auto &token: tokens) {
    expression_string += token.GetTokenString();
  }
  return expression_string;
}

vector<Variable*> EntityFactory::GetVariablesFromExpressionTokens(vector<Token> tokens) {
  vector<Variable*> variables;
  for (auto &token: tokens) {
    if (token.GetTokenTag() == TokenTag::kName) {
      variables.push_back(RetrieveVariable(token.GetTokenString()));
    }
  }
  return variables;
}

vector<ConstantValue*> EntityFactory::GetConstantsFromExpressionTokens(vector<Token> tokens) {
  vector<ConstantValue*> constants;
  for (auto &token: tokens) {
    if (token.GetTokenTag() == TokenTag::kInteger) {
      constants.push_back(CreateConstantValue(token.GetTokenString()));
    }
  }
  return constants;
}

Procedure* EntityFactory::CreateProcedure(std::string proc_name) {
  Procedure* p = new Procedure(new ProcedureName(proc_name));
  proc_list_->push_back(p);
  return p;
}

Variable* EntityFactory::CreateVariable(std::string var_name) {
  Variable* v = new Variable(new VariableName(var_name));
  var_list_->push_back(v);
  return v;
}

ConstantValue* EntityFactory::CreateConstantValue(std::string const_val) {
  ConstantValue* val = new ConstantValue(const_val);
  const_list_->push_back(val);
  return val;
}

/**
 * Tries to retrieve the correct procedure object using the name. If fails, create a new Procedure object.
 */
Procedure* EntityFactory::RetrieveProcedure(std::string proc_name) {
  //TODO: improve the algo

  bool new_proc_ = true;
  Procedure* get_proc_ = CreateProcedure(proc_name);
  for (auto const& i : *proc_list_) {
    if (i->getName() == get_proc_->getName()) {
      //TODO: Create destructor for ProcedureName and Procedure.
      delete get_proc_;
      get_proc_ = i;
      new_proc_ = false;
      break;
    }
  }

  if (!new_proc_)
    proc_list_->pop_back(); //pop back because CreateProcedure push it to the back.

  return get_proc_;
}

/**
 * Tries to retrieve the correct variable object using the name. If fails, create a new Variable object.
 */
Variable* EntityFactory::RetrieveVariable(std::string var_name) {
  //TODO: improve the algo

  bool new_var_ = true;
  Variable* get_var_ = CreateVariable(var_name);
  for (auto const& i : *var_list_) {
    if (i->getName() == get_var_->getName()) {
      //TODO: Create destructor for VariableName and Variable.
      delete get_var_;
      get_var_ = i;
      new_var_ = false;
      break;
    }
  }

  if (!new_var_)
    var_list_->pop_back(); //pop back because CreateVariable push it to the back.

  return get_var_;
}

