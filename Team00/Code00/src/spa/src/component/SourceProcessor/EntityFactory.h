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
  static std::string GetExpression(vector<Token> vector, const std::string& start, const std::string& end);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_
