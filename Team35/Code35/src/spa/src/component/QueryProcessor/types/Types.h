#ifndef AUTOTESTER_TYPES_H
#define AUTOTESTER_TYPES_H

#include <string>
#include <vector>
#include <array>
#include <set>
#include <datatype/DataType.h>
#include <typeinfo>

enum class DesignEntity : unsigned int {
  kStmt = 0,
  kRead = 1,
  kPrint = 2,
  kCall = 3,
  kWhile = 4,
  kIf = 5,
  kAssign = 6,
  kVariable = 7,
  kConstant = 8,
  kProcedure = 9,
  kInvalid = 10,
};

const std::array<DesignEntity, 11> all_design_entities = {
    DesignEntity:: kStmt,
    DesignEntity:: kRead,
    DesignEntity:: kPrint,
    DesignEntity:: kCall,
    DesignEntity:: kWhile,
    DesignEntity:: kIf,
    DesignEntity:: kAssign,
    DesignEntity:: kVariable,
    DesignEntity:: kConstant,
    DesignEntity:: kProcedure,
    DesignEntity:: kInvalid,
};



static DesignEntity GetDesignEntity(std::string reference) {
  if (reference == "stmt") {
    return DesignEntity::kStmt;
  } else if (reference == "read") {
    return DesignEntity::kRead;
  } else if (reference == "print") {
    return DesignEntity::kPrint;
  } else if (reference == "call") {
    return DesignEntity::kCall;
  } else if (reference == "while") {
    return DesignEntity::kWhile;
  } else if (reference == "if") {
    return DesignEntity::kIf;
  } else if (reference == "assign") {
    return DesignEntity::kAssign;
  } else if (reference == "variable") {
    return DesignEntity::kVariable;
  } else if (reference == "constant") {
    return DesignEntity::kConstant;
  } else if (reference == "procedure") {
    return DesignEntity::kProcedure;
  }

  return DesignEntity::kInvalid;
}

enum class PKBRelRefs {
  kFollows,
  kFollowsT,
  kFollowedBy,
  kFollowedByT,
  kParent,
  kParentT,
  kChild,
  kChildT,
  kUsesS,
  kUsesC,
  kUsedByS,
  kUsedByC,
  kModifiesStatement,
  kModifiesContainer,
  kModifiedByStatement,
  kModifiedByContainer,
  kCalls
};

const std::array<PKBRelRefs, 16> pkb_rel_refs = {
    PKBRelRefs::kFollows,
    PKBRelRefs::kFollowsT,
    PKBRelRefs::kFollowedBy,
    PKBRelRefs::kFollowedByT,
    PKBRelRefs::kParent,
    PKBRelRefs::kParentT,
    PKBRelRefs::kChild,
    PKBRelRefs::kChildT,
    PKBRelRefs::kUsesS,
    PKBRelRefs::kUsesC,
    PKBRelRefs::kUsedByS,
    PKBRelRefs::kUsedByC,
    PKBRelRefs::kModifiesStatement,
    PKBRelRefs::kModifiesContainer,
    PKBRelRefs::kModifiedByStatement,
    PKBRelRefs::kModifiedByContainer
};

enum class RelRef {
  kModifiesP,
  kModifiesS,
  kUsesP,
  kUsesS,
  kCalls,
  kCallsT,
  kParent,
  kParentT,
  kFollows,
  kFollowsT,
  kNext,
  kNextT,
  kAffects,
  kAffectsT,
  kWildcard,
  kInvalid,
};

static RelRef GetRelRef(std::string reference) {
  if (reference == "ModifiesP") {
    return RelRef::kModifiesP;
  } else if (reference == "ModifiesS") {
    return RelRef::kModifiesS;
  } else if (reference == "UsesP") {
    return RelRef::kUsesP;
  } else if (reference == "UsesS") {
    return RelRef::kUsesS;
  } else if (reference == "Calls") {
    return RelRef::kCalls;
  } else if (reference == "Calls*") {
    return RelRef::kCallsT;
  } else if (reference == "Parent") {
    return RelRef::kParent;
  } else if (reference == "Parent*") {
    return RelRef::kParentT;
  } else if (reference == "Follows") {
    return RelRef::kFollows;
  } else if (reference == "Follows*") {
    return RelRef::kFollowsT;
  } else if (reference == "Next") {
    return RelRef::kNext;
  } else if (reference == "Next*") {
    return RelRef::kNextT;
  } else if (reference == "Affects") {
    return RelRef::kAffects;
  } else if (reference == "Affects*") {
    return RelRef::kAffectsT;
  }
  // TODO: Throw an error if this line is reached.
  return RelRef::kInvalid;
}

class Synonym {
 private:
  std::string name;
  DesignEntity type;
 public:
  Synonym() {};
  Synonym(std::string name, DesignEntity type) : name(name), type(type) {};
  std::string GetName() { return name; };
  DesignEntity GetType() { return type; };
};

struct Clause {
  std::string left_hand_side;
  std::string right_hand_side;
  virtual std::string getType() { return ""; };
  virtual bool isEqual(Clause toObj) { return 1; };
  virtual ~Clause() {};
};

struct SuchThat : Clause {
  RelRef rel_ref;
  bool left_is_synonym;
  bool right_is_synonym;
  SuchThat() {};
  SuchThat(std::string lhs, std::string rhs, RelRef rf, bool lhs_is_syn, bool rhs_is_syn) {
    left_hand_side = lhs;
    right_hand_side = rhs;
    rel_ref = rf;
    left_is_synonym = lhs_is_syn;
    right_is_synonym = rhs_is_syn;
  }
  std::string getType() const { return typeid(this).name(); }
  bool isEqual(Clause* toObj) {
    if (this->getType() == toObj->getType()) {
      SuchThat* obj = (SuchThat*) & toObj;
      return (
          this->left_hand_side == obj->left_hand_side &&
              this->right_hand_side == obj->right_hand_side &&
              this->rel_ref == obj->rel_ref &&
              this->left_is_synonym == obj->left_is_synonym &&
              this->right_is_synonym == obj->right_is_synonym
      );
    } else {
      return false;
    }
  }
};

struct Pattern : Clause {
  std::string assign_synonym;
  bool left_is_synonym;
  bool is_exact = false;
  Pattern() {};
  Pattern(std::string lhs, std::string rhs, std::string assn_syn, bool lhs_is_syn, bool is_exact) {
    left_hand_side = lhs;
    right_hand_side = rhs;
    assign_synonym = assn_syn;
    left_is_synonym = lhs_is_syn;
    is_exact = is_exact;
  }
  std::string getType() const { return typeid(this).name(); }
  bool isEqual(Clause* toObj) {
    if (this->getType() == toObj->getType()) {
      Pattern* obj = (Pattern*) & toObj;
      return (
          this->left_hand_side == obj->left_hand_side &&
              this->right_hand_side == obj->right_hand_side &&
              this->assign_synonym == obj->assign_synonym &&
              this->left_is_synonym == obj->left_is_synonym &&
              this->is_exact == obj->is_exact
      );
    } else {
      return false;
    }
  }
};

class Group {
 private:
  std::vector<Clause*> clauses;
  bool has_target_synonym;
 public:
  Group(std::vector<Clause*> clauses, bool has_target_synonym) :
      has_target_synonym(has_target_synonym), clauses(clauses) {};
  bool AddClauseToVector(Clause* clause);
  std::vector<Clause*> GetClauses();
  bool ContainsTargetSynonym();
};

#endif //AUTOTESTER_TYPES_H
