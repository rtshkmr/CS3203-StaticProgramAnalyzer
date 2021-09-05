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
  static Entity* CreateEntities(std::vector<Token> tokens);
 private:
  static vector<Token> GetExpressionTokens(vector<Token> tokens, TokenTag startTag, TokenTag endTag);
  static string ConvertTokensToString(vector<Token> tokens);
  static vector<Variable*> GetVariablesFromExpressionTokens(vector<Token> tokens);
  static vector<ConstantValue*> GetConstantsFromExpressionTokens(vector<Token> tokens);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_
