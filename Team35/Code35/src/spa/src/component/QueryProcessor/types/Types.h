#ifndef AUTOTESTER_TYPES_H
#define AUTOTESTER_TYPES_H

#include <string>
#include <vector>
#include <datatype/DataType.h>
#include <typeinfo>

enum class DesignEntity {
  kStmt,
  kRead,
  kPrint,
  kCall,
  kWhile,
  kIf,
  kAssign,
  kVariable,
  kConstant,
  kProcedure,
  kInvalid
};

DesignEntity GetDesignEntity(std::string reference);

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
  kInvalid
};

RelRef GetRelRef(std::string reference);

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
