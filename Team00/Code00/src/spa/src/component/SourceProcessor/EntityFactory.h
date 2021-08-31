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
  EntityFactory();
  void init(int* p_sub); // todo: set pointer to psub in order to  establish bi-directional relationship b/w the two.
  //                              cause EF needs to set flags within helper stacks within psub.
  std::vector<Entity> createEntities(const std::vector<Token>& tokens);
 private:
  int* p_sub;
  Entity create_enitity(Token& token);
};

#endif //AUTOTESTER_TEAM00_CODE00_SRC_SPA_SRC_COMPONENT_SOURCEPROCESSOR_ENTITYFACTORY_H_
