/**
 * EntityFactory is responsible for creating the Entity nodes for the AST from the tokenized statement provided.
 * Entity can be an Abstract/Parent class.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_

#include <model/Entity.h>

class EntityFactory {
 public:
  EntityFactory() = default;
  EntityFactory(std::list<Procedure*>* proc_list,
                std::list<Variable*>* var_list,
                std::list<ConstantValue*>* const_list);
  Entity* CreateEntities(std::vector<Token> tokens);
  static std::vector<Token> GetExpressionTokens(std::vector<Token> tokens, TokenTag start_tag, TokenTag end_tag);
 private:
  //attribute
  std::list<Procedure*>* proc_list_;
  std::list<Variable*>* var_list_;
  std::list<ConstantValue*>* const_list_;

  Procedure* CreateProcedure(std::string proc_name);
  Variable* CreateVariable(std::string var_name);
  ConstantValue* CreateConstantValue(std::string const_val);
  Procedure* RetrieveProcedure(std::string proc_name);
  Variable* RetrieveVariable(std::string var_name);

  Entity* CreateReadEntity(std::vector<Token> tokens);
  Entity* CreatePrintEntity(std::vector<Token> tokens);
  Entity* CreateCallEntity(std::vector<Token> tokens);
  Entity* CreateConditionalEntity(std::vector<Token> tokens, TokenTag entity_type);
  Entity* CreateAssignEntity(std::vector<Token> tokens);

  static std::string ConvertTokensToString(std::vector<Token> tokens);
  std::vector<Variable*> GetVariablesFromExpressionTokens(std::vector<Token> tokens);
  std::vector<ConstantValue*> GetConstantsFromExpressionTokens(std::vector<Token> tokens);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_
