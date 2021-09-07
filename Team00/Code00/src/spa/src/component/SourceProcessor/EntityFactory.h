/**
 * EntityFactory is responsible for creating the Entity nodes for the AST from the tokenized statement provided.
 * Entity can be an Abstract/Parent class.
 */
#ifndef AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_
#define AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_

#include <model/Entity.h>
#include <vector>
using namespace std;
class EntityFactory {
 public:
  EntityFactory() = default;
  EntityFactory(std::list<Procedure*>* proc_list, std::list<Variable*>* var_list, std::list<ConstantValue*>* const_list);
  Entity* CreateEntities(std::vector<Token> tokens);
 private:
  //attribute
  std::list<Procedure*>* proc_list_;
  std::list<Variable*>* var_list_;
  std::list<ConstantValue*>* const_list_;

  Procedure* CreateProcedure(std::string proc_name);
  Variable*  CreateVariable(std::string var_name);
  ConstantValue* CreateConstantValue(std::string const_val);
  Procedure* RetrieveProcedure(std::string proc_name);
  Variable*  RetrieveVariable(std::string var_name);

  Entity* CreateConditionalEntity(vector<Token> tokens, TokenTag entity_type);
  Entity* CreateAssignEntity(vector<Token> tokens);
  vector<Token> GetExpressionTokens(vector<Token> tokens, TokenTag startTag, TokenTag endTag);
  static string ConvertTokensToString(vector<Token> tokens);
  vector<Variable*> GetVariablesFromExpressionTokens(vector<Token> tokens);
  vector<ConstantValue*> GetConstantsFromExpressionTokens(vector<Token> tokens);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_
