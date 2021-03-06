#include <stdexcept>
#include <utility>
#include "EntityFactory.h"
#include "model/Statement.h"

using std::string;
using std::vector;

EntityFactory::EntityFactory(std::list<Procedure*>* proc_list,
                             std::list<Variable*>* var_list,
                             std::list<Constant*>* const_list) {
  proc_list_ = proc_list;
  var_list_ = var_list;
  const_list_ = const_list;
}

/**
 * Creates Entities out of the given tokens.
 * Must assume that the tokens are in the form of a statement e.g. the 1st word is the keyword
 * and that the tokenized statement is syntactically correct
 */
Entity* EntityFactory::CreateEntity(vector<Token> tokens) {
  Token first_token = tokens.front();
  switch (first_token.GetTokenTag()) {
    case TokenTag::kProcedureKeyword: {
      std::string proc_token_string = tokens[1].GetTokenString();
      return RetrieveProcedure(proc_token_string);
    }
    case TokenTag::kReadKeyword: {
      return CreateReadEntity(tokens);
    }
    case TokenTag::kPrintKeyword: {
      return CreatePrintEntity(tokens);
    }
    case TokenTag::kCallKeyword: {
      return CreateCallEntity(tokens);
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
      return CreateAssignEntity(tokens);
    }
    default:throw std::invalid_argument("Tokens cannot be made into entity in EF.");
  }
}

Entity* EntityFactory::CreateReadEntity(std::vector<Token> tokens) {
  std::string read_var_string = tokens[1].GetTokenString();
  return new ReadEntity(RetrieveVariable(read_var_string));
}

Entity* EntityFactory::CreatePrintEntity(std::vector<Token> tokens) {
  std::string print_var_string = tokens[1].GetTokenString();
  return new PrintEntity(RetrieveVariable(print_var_string));
}

Entity* EntityFactory::CreateCallEntity(std::vector<Token> tokens) {
  std::string call_proc_string = tokens[1].GetTokenString();
  return new CallEntity(RetrieveProcedure(call_proc_string));
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

  vector<Token> expression_tokens = GetExpressionTokens(tokens, TokenTag::kOpenBracket, TokenTag::kCloseBracket);
  std::string expression_string = ConvertTokensToString(expression_tokens);
  vector<Variable*> expression_variables = GetVariablesFromExpressionTokens(expression_tokens);
  vector<Constant*> expression_constants = GetConstantsFromExpressionTokens(expression_tokens);

  if (entity_type == TokenTag::kIfKeyword) {
    return new IfEntity(expression_string, expression_variables, expression_constants);
  } else if (entity_type == TokenTag::kWhileKeyword) {
    return new WhileEntity(expression_string, expression_variables, expression_constants);
  } else {
    throw std::invalid_argument("Entity type for creating Conditional Entity is wrong.");
  }
}

/**
 * Creates an AssignEntity and extracts the string of the expression, and the expression's variables and
 * ConstantValues.
 *
 * @param tokens A tokenized statement.
 * @return AssignEntity.
 */
Entity* EntityFactory::CreateAssignEntity(vector<Token> tokens) {

  vector<Token> expression_tokens = GetExpressionTokens(tokens, TokenTag::kAssignmentOperator, TokenTag::kSemicolon);
  std::string expression_string = ConvertTokensToString(expression_tokens);
  vector<Variable*> expression_variables = GetVariablesFromExpressionTokens(expression_tokens);
  vector<Constant*> expression_constants = GetConstantsFromExpressionTokens(expression_tokens);

  return new AssignEntity(
      RetrieveVariable(tokens.front().GetTokenString()),
      expression_string,
      expression_variables,
      expression_constants);
}

/**
 * Gets Tokens that make an expression,
 * from the vector of tokens passed in which is in the form of a whole tokenized statement.
 * The expression is starts after the Token with a TokenTag of startTag and ends before the Token with a TokenTag of
 * endTag.
 * There must be some Token between start and end tag.
 *
 * @param tokens Tokens that represent a statement.
 * @param start_tag The TokenTag which indicates that the next token is the start of an expression.
 * @param end_tag The TokenTag which indicates that the previous token is the end of an expression.
 * @return Vector of tokens which represents the expression alone.
 */
vector<Token> EntityFactory::GetExpressionTokens(vector<Token> tokens, TokenTag start_tag, TokenTag end_tag) {
  int start_iter = Token::GetFirstMatchingTokenIdx(tokens, start_tag);
  int end_iter = Token::GetLastMatchingTokenIdx(tokens, end_tag);
  if (start_iter < 0 || end_iter < 0) {
    throw std::invalid_argument("EF: Start or end tag not found.\n");
  }
  if (start_iter + 1 >= end_iter) {
    throw std::invalid_argument("EF:There is nothing between start and end tag.\n");
  }

  // collecting tokens
  vector<Token> expression_tokens;
  for (int i = start_iter + 1; i < end_iter; ++i) {
    expression_tokens.push_back(tokens[i]);
  }

  return expression_tokens;
}

string EntityFactory::ConvertTokensToString(const vector<Token>& tokens) {
  std::string expression_string;
  for (auto& token: tokens) {
    expression_string += token.GetTokenString();
  }
  return expression_string;
}

vector<Variable*> EntityFactory::GetVariablesFromExpressionTokens(const vector<Token>& tokens) {
  vector<Variable*> variables;
  for (auto& token: tokens) {
    if (token.GetTokenTag() == TokenTag::kName) {
      Variable* curr_var = RetrieveVariable(token.GetTokenString());
      auto iter = std::find(variables.begin(), variables.end(), curr_var);
      if (iter == variables.end()) {
        // add curr_var if it is not found in variables
        variables.push_back(curr_var);
      }
    }
  }
  return variables;
}

vector<Constant*> EntityFactory::GetConstantsFromExpressionTokens(const vector<Token>& tokens) {
  vector<Constant*> constants;
  for (auto& token: tokens) {
    if (token.GetTokenTag() == TokenTag::kInteger) {
      Constant* curr_const = CreateConstant(token.GetTokenString());
      auto iter = std::find(constants.begin(), constants.end(), curr_const);
      if (iter == constants.end()) {
        // add curr_const if it is not found in constants
        constants.push_back(curr_const);
      }
    }
  }
  return constants;
}

Procedure* EntityFactory::CreateProcedure(std::string proc_name) {
  auto* p = new Procedure(new ProcedureName(std::move(proc_name)));
  proc_list_->push_back(p);
  return p;
}

Variable* EntityFactory::CreateVariable(std::string var_name) {
  auto* v = new Variable(new VariableName(std::move(var_name)));
  var_list_->push_back(v);
  return v;
}

Constant* EntityFactory::CreateConstant(const std::string& const_val) {
  auto* val = new Constant(new ConstantValue(const_val));
  const_list_->push_back(val);
  return val;
}

/**
 * Tries to retrieve the correct procedure object using the name. If fails, create a new Procedure object.
 */
Procedure* EntityFactory::RetrieveProcedure(std::string proc_name) {
  ProcedureName temp_proc_name = ProcedureName(proc_name);
  for (auto const& proc: * proc_list_) {
    if (* proc->GetName() == temp_proc_name) { // uses the overloaded ==
      return proc;
    }
  }

  return CreateProcedure(std::move(proc_name));
}

/**
 * Tries to retrieve the correct variable object using the name. If fails, create a new Variable object.
 */
Variable* EntityFactory::RetrieveVariable(std::string var_name) {
  VariableName temp_var_name = VariableName(var_name);
  for (auto const& var: * var_list_) {
    if (* var->GetVariableName() == temp_var_name) { // uses the overloaded ==
      return var;
    }
  }

  return CreateVariable(std::move(var_name));
}
