#include <util/Utility.h>
#include "RuntimeExtractor.h"

RuntimeExtractor::RuntimeExtractor(PKB* pkb) {
  pkb_ = pkb;
  next_bip_extractor_ = NextBipExtractor(pkb);
  next_bip_t_extractor_ = NextBipTExtractor(pkb);
}

std::vector<Entity*> RuntimeExtractor::GetRelationship(PKBRelRefs ref, std::string target) {
  int target_num = Utility::ConvertStringToInt(target);
  switch (ref) {
    case PKBRelRefs::kNextT: return GetNextT(target_num);
    case PKBRelRefs::kPreviousT: return GetPrevT(target_num);
    case PKBRelRefs::kAffects: return GetAffects(target_num);
    case PKBRelRefs::kAffectedBy: return GetAffectedBy(target_num);
    case PKBRelRefs::kAffectsT: return GetAffectsT(target_num);
    case PKBRelRefs::kAffectedByT: return GetAffectedByT(target_num);
    case PKBRelRefs::kNextBip: return GetNextBip(target);
    case PKBRelRefs::kPrevBip: return GetPrevBip(target);
    case PKBRelRefs::kNextBipT: return GetNextBipT(target);
    case PKBRelRefs::kPrevBipT: return GetPrevBipT(target);
    default: return std::vector<Entity*>{};
  }
}

std::vector<Entity*> RuntimeExtractor::GetFirstEntityOfRelationship(PKBRelRefs ref, DesignEntity de) {
  switch (ref) {
    case PKBRelRefs::kNextT: return GetNextT(de);
    case PKBRelRefs::kPreviousT: return GetPrevT(de);
    case PKBRelRefs::kAffects: return GetAffects(de);
    case PKBRelRefs::kAffectedBy: return GetAffectedBy(de);
    case PKBRelRefs::kAffectsT: return GetAffectsT(de);
    case PKBRelRefs::kAffectedByT: return GetAffectedByT(de);
    case PKBRelRefs::kNextBip: return GetNextBip(de);
    case PKBRelRefs::kPrevBip: return GetPrevBip(de);
    default: return std::vector<Entity*>{};
  }
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetRelationshipByTypes(PKBRelRefs ref,
                                                                                   DesignEntity first,
                                                                                   DesignEntity second) {
  switch (ref) {
    case PKBRelRefs::kNextT: return GetNextT(first, second);
    case PKBRelRefs::kPreviousT: return GetPrevT(first, second);
    case PKBRelRefs::kAffects: return GetAffects(first, second);
    case PKBRelRefs::kAffectedBy: return GetAffectedBy(first, second);
    case PKBRelRefs::kAffectsT: return GetAffectsT(first, second);
    case PKBRelRefs::kAffectedByT: return GetAffectedByT(first, second);
    case PKBRelRefs::kNextBip: return GetNextBip(first, second);
    case PKBRelRefs::kPrevBip: return GetPrevBip(first, second);
    default: return std::vector<std::tuple<Entity*, Entity*>>{};
  }
}

bool RuntimeExtractor::HasRelationship(PKBRelRefs ref) {
  switch (ref) {
    case PKBRelRefs::kNextT:  // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext);
    case PKBRelRefs::kAffects: return HasAffects();
    case PKBRelRefs::kAffectedBy: return HasAffectedBy();
    case PKBRelRefs::kAffectsT: return HasAffects();
    case PKBRelRefs::kAffectedByT: return HasAffectedBy();
    case PKBRelRefs::kNextBip: return HasNextBip();
    case PKBRelRefs::kPrevBip: return HasPrevBip();
    default: return false;
  }
}

bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, std::string first) {
  int target_num = Utility::ConvertStringToInt(first);
  switch (ref) {
    case PKBRelRefs::kNextT: // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext, first);
    case PKBRelRefs::kAffects: return HasAffects(target_num);
    case PKBRelRefs::kAffectedBy: return HasAffectedBy(target_num);
    case PKBRelRefs::kAffectsT: return HasAffectsT(target_num);
    case PKBRelRefs::kAffectedByT: return HasAffectedByT(target_num);
    case PKBRelRefs::kNextBip: return HasNextBip(first);
    case PKBRelRefs::kPrevBip: return HasPrevBip(first);
    default: return false;
  }
}

bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, std::string first, std::string second) {
  int first_num = Utility::ConvertStringToInt(first);
  int second_num = Utility::ConvertStringToInt(second);
  switch (ref) {
    case PKBRelRefs::kNextT: return HasNextT(first_num, second_num);
    case PKBRelRefs::kPreviousT: return HasNextT(second_num, first_num);
    case PKBRelRefs::kAffects: return HasAffects(first_num, second_num);
    case PKBRelRefs::kAffectedBy: return HasAffectedBy(first_num, second_num);
    case PKBRelRefs::kAffectsT: return HasAffectsT(first_num, second_num);
    case PKBRelRefs::kAffectedByT: return HasAffectedByT(first_num, second_num);
    case PKBRelRefs::kNextBip: return HasNextBip(first, second);
    case PKBRelRefs::kPrevBip: return HasNextBip(second, first);
    default: return false;
  }
}

bool RuntimeExtractor::HasRelationship(PKBRelRefs ref, DesignEntity first, DesignEntity secon) {
  switch (ref) {
    case PKBRelRefs::kNextT:  // fallthrough
    case PKBRelRefs::kPreviousT: return pkb_->HasRelationship(PKBRelRefs::kNext);
    case PKBRelRefs::kAffects: return HasAffects();
    case PKBRelRefs::kAffectedBy: return HasAffectedBy();
    case PKBRelRefs::kAffectsT: return HasAffects();
    case PKBRelRefs::kAffectedByT: return HasAffectedBy();
    case PKBRelRefs::kNextBip: return HasNextBip();
    case PKBRelRefs::kPrevBip: return HasPrevBip();
    default: return false;
  }
}

std::vector<Entity*> RuntimeExtractor::GetNextT(int target) {
  return next_t_extractor_.GetNextT(target, std::vector<Procedure*>{}, std::vector<Statement*>{});
  // todo update with pkb api
}

std::vector<Entity*> RuntimeExtractor::GetPrevT(int target) {
  return next_t_extractor_.GetPrevT(target, std::vector<Procedure*>{}, std::vector<Statement*>{});
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

std::vector<Entity*> RuntimeExtractor::GetNextBip(std::string target) {
  return next_bip_extractor_.GetNextBip(target);
}

std::vector<Entity*> RuntimeExtractor::GetPrevBip(std::string target) {
  return next_bip_extractor_.GetPrevBip(target);
}

std::vector<Entity*> RuntimeExtractor::GetNextBipT(std::string target) {
  return next_bip_t_extractor_.GetRelationship(RelDirection::kForward, target);
}

std::vector<Entity*> RuntimeExtractor::GetPrevBipT(std::string target) {
  return next_bip_t_extractor_.GetRelationship(RelDirection::kReverse, target);
}

std::vector<Entity*> RuntimeExtractor::GetNextT(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_t_extractor_.GetAllNextTLHS(std::vector<Procedure*>{}, std::vector<Statement*>{});
}

std::vector<Entity*> RuntimeExtractor::GetPrevT(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_t_extractor_.GetAllNextTRHS(std::vector<Procedure*>{}, std::vector<Statement*>{});
}

std::vector<Entity*> RuntimeExtractor::GetAffects(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectedBy(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectsT(DesignEntity de) { return std::vector<Entity*>(); }
std::vector<Entity*> RuntimeExtractor::GetAffectedByT(DesignEntity de) { return std::vector<Entity*>(); }

std::vector<Entity*> RuntimeExtractor::GetNextBip(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_bip_extractor_.GetAllNextBipRHS();
}

std::vector<Entity*> RuntimeExtractor::GetPrevBip(DesignEntity de) {
  if (de != DesignEntity::kStmt && de != DesignEntity::kProgLine) {
    return std::vector<Entity*>{};
  }
  return next_bip_extractor_.GetAllNextBipLHS();
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextT(DesignEntity first, DesignEntity second) {
  bool not_stmt = first != DesignEntity::kStmt || second != DesignEntity::kStmt;
  bool not_prog_line = first != DesignEntity::kProgLine || second != DesignEntity::kProgLine;
  if (not_stmt && not_prog_line) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_t_extractor_.GetAllNextT(std::vector<Procedure*>{}, std::vector<Statement*>{});
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetPrevT(DesignEntity first, DesignEntity second) {
  bool not_stmt = first != DesignEntity::kStmt || second != DesignEntity::kStmt;
  bool not_prog_line = first != DesignEntity::kProgLine || second != DesignEntity::kProgLine;
  if (not_stmt && not_prog_line) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_t_extractor_.GetAllPrevT(std::vector<Procedure*>{}, std::vector<Statement*>{});
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

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetNextBip(DesignEntity first, DesignEntity second) {
  bool not_stmt = first != DesignEntity::kStmt || second != DesignEntity::kStmt;
  bool not_prog_line = first != DesignEntity::kProgLine || second != DesignEntity::kProgLine;
  if (not_stmt && not_prog_line) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_bip_extractor_.GetAllNextBip();
}

std::vector<std::tuple<Entity*, Entity*>> RuntimeExtractor::GetPrevBip(DesignEntity first, DesignEntity second) {
  bool not_stmt = first != DesignEntity::kStmt || second != DesignEntity::kStmt;
  bool not_prog_line = first != DesignEntity::kProgLine || second != DesignEntity::kProgLine;
  if (not_stmt && not_prog_line) {
    return std::vector<std::tuple<Entity*, Entity*>>{};
  }
  return next_bip_extractor_.GetAllPrevBip();
}

bool RuntimeExtractor::HasAffects() { return false; }
bool RuntimeExtractor::HasAffectedBy() { return false; }

bool RuntimeExtractor::HasNextBip() {
  return next_bip_extractor_.HasNextBip();
}

bool RuntimeExtractor::HasPrevBip() {
  return next_bip_extractor_.HasPrevBip();
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

bool RuntimeExtractor::HasNextBip(std::string first) {
  return next_bip_extractor_.HasNextBip(first);
}

bool RuntimeExtractor::HasPrevBip(std::string first) {
  return next_bip_extractor_.HasPrevBip(first);
}

bool RuntimeExtractor::HasNextT(int first, int second) {
  return next_t_extractor_.HasNextT(first, second,
                                    std::vector<Procedure*>{},
                                    std::vector<Statement*>{});
}

bool RuntimeExtractor::HasAffects(int first, int second) { return false; }
bool RuntimeExtractor::HasAffectedBy(int first, int second) { return false; }
bool RuntimeExtractor::HasAffectsT(int first, int second) { return false; }
bool RuntimeExtractor::HasAffectedByT(int first, int second) { return false; }

bool RuntimeExtractor::HasNextBip(std::string first, std::string second) {
  return next_bip_extractor_.HasNextBip(first, second);
}

void RuntimeExtractor::Delete() {
  next_t_extractor_.Delete();
  affects_extractor_.Delete();
  affects_t_extractor_.Delete();
}

bool RuntimeExtractor::IsRuntimeRelationship(PKBRelRefs ref) {
  return runtime_relationships.count(ref);
}
