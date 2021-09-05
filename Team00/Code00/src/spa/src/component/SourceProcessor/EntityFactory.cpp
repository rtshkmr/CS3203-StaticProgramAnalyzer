//
// Created by mkrit on 31/8/2021.
//

#include "EntityFactory.h"
EntityFactory::EntityFactory() {
  p_sub_ = nullptr;
}


/*
 * Since the EntityFactory has to have information across multiple lines, we'd need to
 * depend on the PSub to read state from and hence this Init function adds a pointer to that Psub.
 */
void EntityFactory::Init(int* psub) {
  if (!this->p_sub_) {
    this->p_sub_ = psub;
  }
}

/*
 vector<Entity> EntityFactory::CreateEntities(const vector<Token>& tokens) {
  for (auto t:tokens) {
    Entity entity = CreateEntity(t);
  }
  return {};
}
*/

Entity EntityFactory::CreateEntity(Token& token) {
  // todo add logic for creating a single entity
  return {};
}

