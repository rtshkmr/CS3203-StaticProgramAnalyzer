#include "RuntimeExtractor.h"
#include "NextTExtractor.h"

RuntimeExtractor::RuntimeExtractor(PKB* pkb) {
  pkb_ = pkb;
}

std::vector<Entity*> RuntimeExtractor::GetRelationships(RelRef ref, std::string target) {
  switch (ref) {
    case RelRef::kNextT:return NextTExtractor::GetNextT(target);
    case RelRef::kAffects: break;
    case RelRef::kAffectsT: break;
  }
  return std::vector<Entity*>();
}

bool RuntimeExtractor::HasAffects() {
  return false;
}

bool RuntimeExtractor::HasAffectsT() {
  return false;
}
