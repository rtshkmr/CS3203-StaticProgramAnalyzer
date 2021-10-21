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

std::vector<Entity*> RuntimeExtractor::GetNextT(DesignEntity de) {}
std::vector<Entity*> RuntimeExtractor::GetPrevT(DesignEntity de) {}
std::vector<Entity*> RuntimeExtractor::GetAffects(DesignEntity de) {}
std::vector<Entity*> RuntimeExtractor::GetAffectedBy(DesignEntity de) {}
std::vector<Entity*> RuntimeExtractor::GetAffectsT(DesignEntity de) {}
std::vector<Entity*> RuntimeExtractor::GetAffectedByT(DesignEntity de) {}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextT(DesignEntity first, DesignEntity second) {}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetPrevT(DesignEntity first, DesignEntity second) {}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffects(DesignEntity first, DesignEntity second) {}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectedBy(DesignEntity first, DesignEntity second) {}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectsT(DesignEntity first, DesignEntity second) {}
std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetAffectedByT(DesignEntity first, DesignEntity second) {}

bool RuntimeExtractor::HasNextT() {}
bool RuntimeExtractor::HasPrevT() {}
bool RuntimeExtractor::HasAffects() {}
bool RuntimeExtractor::HasAffectedBy() {}
bool RuntimeExtractor::HasAffectsT() {}
bool RuntimeExtractor::HasAffectedByT() {}

bool RuntimeExtractor::HasNextT(DesignEntity first, DesignEntity second) {}
bool RuntimeExtractor::HasPrevT(DesignEntity first, DesignEntity second) {}
bool RuntimeExtractor::HasAffects(DesignEntity first, DesignEntity second) {}
bool RuntimeExtractor::HasAffectedBy(DesignEntity first, DesignEntity second) {}
bool RuntimeExtractor::HasAffectsT(DesignEntity first, DesignEntity second) {}
bool RuntimeExtractor::HasAffectedByT(DesignEntity first, DesignEntity second) {}

bool RuntimeExtractor::HasNextT(std::string first, std::string second) {}
bool RuntimeExtractor::HasPrevT(std::string first, std::string second) {}
bool RuntimeExtractor::HasAffects(std::string first, std::string second) {}
bool RuntimeExtractor::HasAffectedBy(std::string first, std::string second) {}
bool RuntimeExtractor::HasAffectsT(std::string first, std::string second) {}
bool RuntimeExtractor::HasAffectedByT(std::string first, std::string second) {}
