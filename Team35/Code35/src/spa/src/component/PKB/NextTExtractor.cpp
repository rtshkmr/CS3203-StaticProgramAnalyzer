#include "NextTExtractor.h"

/**
 * Extracts list of Next* of the target from the CFG.
 * @param target String of the statement number to look for.
 * @return List of Entities that are Next* of the target.
 *
 * pseudocode
 * get the block of the target stmt
 * recurse into next blocks if not visited
 * return next* of recursed block
 * add list of next* to this block
 *
 * get list of next from hashmap
 * add to next* of this block and visited blocks
 * recurse into next blocks if not visited
 */
std::vector<Entity*> NextTExtractor::GetNextT(std::string target) {
  return std::vector<Entity*>();
}
