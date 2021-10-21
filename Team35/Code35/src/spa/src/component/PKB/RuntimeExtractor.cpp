#include "RuntimeExtractor.h"

RuntimeExtractor::RuntimeExtractor(PKB* pkb) {
  pkb_ = pkb;
}

std::vector<Entity*> RuntimeExtractor::GetNextT(std::string target) {
  return next_t_extractor_.GetNextT(target,
                                   std::vector<Procedure*>{},
                                   std::vector<Statement*>{});  // todo update with pkb api
}

std::vector<Entity*> RuntimeExtractor::GetPrevT(std::string target) {
  return next_t_extractor_.GetPrevT(target,
                                    std::vector<Procedure*>{},
                                    std::vector<Statement*>{});
}

std::vector<Entity*> RuntimeExtractor::GetAffects(std::string target) {
  return affects_extractor_.GetAffects(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectedBy(std::string target) {
  return affects_extractor_.GetAffectedBy(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectsT(std::string target) {
  return affects_t_extractor_.GetAffectsT(target);
}

std::vector<Entity*> RuntimeExtractor::GetAffectedByT(std::string target) {
  return affects_t_extractor_.GetAffectedByT(target);
}

std::vector<Entity*> RuntimeExtractor::GetNextT(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetPrevT(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffects(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectedBy(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectsT(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectedByT(DesignEntity de) { return std::vector<Entity*>(); }

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextT(DesignEntity first, DesignEntity second) {
  return std::vector<std::tuple<Entity*, Entity*>>();
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

bool RuntimeExtractor::HasNextT() { return false; }
bool RuntimeExtractor::HasPrevT() { return false; }
bool RuntimeExtractor::HasAffects() { return false; }
bool RuntimeExtractor::HasAffectedBy() { return false; }
bool RuntimeExtractor::HasAffectsT() { return false; }
bool RuntimeExtractor::HasAffectedByT() { return false; }

bool RuntimeExtractor::HasNextT(std::string first, std::string second) { return false; }
bool RuntimeExtractor::HasPrevT(std::string first, std::string second) { return false; }
bool RuntimeExtractor::HasAffects(std::string first, std::string second) { return false; }
bool RuntimeExtractor::HasAffectedBy(std::string first, std::string second) { return false; }
bool RuntimeExtractor::HasAffectsT(std::string first, std::string second) { return false; }
bool RuntimeExtractor::HasAffectedByT(std::string first, std::string second) { return false; }

void RuntimeExtractor::Delete() {
  next_t_extractor_.Delete();
  affects_extractor_.Delete();
  affects_t_extractor_.Delete();
}
