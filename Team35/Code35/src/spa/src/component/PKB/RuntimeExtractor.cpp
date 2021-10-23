#include "RuntimeExtractor.h"

RuntimeExtractor::RuntimeExtractor(PKB* pkb) {
  pkb_ = pkb;
}

std::vector<Entity*> RuntimeExtractor::GetNextT(int target) {
  return next_t_extractor_.GetNextT(target,
                                    std::vector<Procedure*>{},
                                    std::vector<Statement*>{});  // todo update with pkb api
}

std::vector<Entity*> RuntimeExtractor::GetPrevT(int target) {
  return std::vector<Entity*>();
}

std::vector<Entity*> RuntimeExtractor::GetAffects(int target) {
  return affects_extractor_.GetAffects(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectedBy(int target) {
  return affects_extractor_.GetAffectedBy(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectsT(int target) {
  return affects_t_extractor_.GetAffectsT(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectedByT(int target) {
  return affects_t_extractor_.GetAffectedByT(target);
}

std::vector<Entity*> RuntimeExtractor::GetNextT(DesignEntity de) {
  if (de != DesignEntity::kStmt) {
    return std::vector<Entity*>{};
  }
  return next_t_extractor_.GetAllNextTLHS(std::vector<Procedure*>{}, std::vector<Statement*>{});
}

std::vector<Entity*> RuntimeExtractor::GetPrevT(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffects(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectedBy(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectsT(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectedByT(DesignEntity de) { return std::vector<Entity*>(); }

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextT(DesignEntity first, DesignEntity second) {
  if (first != DesignEntity::kStmt || second != DesignEntity::kStmt) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_t_extractor_.GetAllNextT(std::vector<Procedure*>{}, std::vector<Statement*>{});
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetPrevT(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffects(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectedBy(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectsT(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectedByT(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
}

bool RuntimeExtractor::HasAffects() { return false; }
bool RuntimeExtractor::HasAffectedBy() { return false; }

bool RuntimeExtractor::HasNextT(int target) {
  return false;
}

bool RuntimeExtractor::HasPrevT(int target) {
  return false;
}

bool RuntimeExtractor::HasAffects(int target) {
  return false;
}

bool RuntimeExtractor::HasAffectedBy(int target) {
  return false;
}

bool RuntimeExtractor::HasAffectsT(int target) {
  return false;
}

bool RuntimeExtractor::HasAffectedByT(int target) {
  return false;
}

bool RuntimeExtractor::HasNextT(int first, int second) {
  return next_t_extractor_.HasNextT(first, second,
                                    std::vector<Procedure*>{},
                                    std::vector<Statement*>{});
}

bool RuntimeExtractor::HasPrevT(int first, int second) { return false; }
bool RuntimeExtractor::HasAffects(int first, int second) { return false; }
bool RuntimeExtractor::HasAffectedBy(int first, int second) { return false; }
bool RuntimeExtractor::HasAffectsT(int first, int second) { return false; }
bool RuntimeExtractor::HasAffectedByT(int first, int second) { return false; }

void RuntimeExtractor::Delete() {
  next_t_extractor_.Delete();
  affects_extractor_.Delete();
  affects_t_extractor_.Delete();
}
