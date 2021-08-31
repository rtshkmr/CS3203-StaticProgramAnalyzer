//
// Created by mkrit on 31/8/2021.
//

#include "EntityFactory.h"
EntityFactory::EntityFactory() {
  p_sub = nullptr;
}


/*
 * Since the EntityFactory has to have information across multiple lines, we'd need to
 * depend on the PSub to read state from and hence this init function adds a pointer to that Psub.
 */
void EntityFactory::init(int* psub) {
  if (!this->p_sub) {
    this->p_sub = psub;
  }
}
vector<Entity> EntityFactory::createEntities(const vector<Token>& tokens) {
  for (auto t:tokens) {
    Entity entity = create_enitity(t);
  }
  return {};
}

Entity EntityFactory::create_enitity(Token& token) {
  // todo add logic for creating a single entity
  return {};
}

