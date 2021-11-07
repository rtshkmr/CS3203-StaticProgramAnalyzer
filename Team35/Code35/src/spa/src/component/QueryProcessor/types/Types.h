#ifndef AUTOTESTER_TYPES_H
#define AUTOTESTER_TYPES_H

#include <string>
#include <vector>
#include <array>
#include <unordered_set>
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
  kProgLine = 10,
  kInvalid = 11
};

enum class PKBRelRefs {
  kFollows,
  kFollowsT,
  kFollowedBy,
  kFollowedByT,
  kParent,
  kParentT,
  kChild,
  kChildT,
  kCalls,
  kCalledBy,
  kCallsT,
  kCalledByT,
  kUses,
  kUsedBy,
  kModifies,
  kModifiedBy,
  kNext,
  kPrevious,
  kNextT,
  kPreviousT,
  kAffects,
  kAffectedBy,
  kAffectsT,
  kAffectedByT,
  kNextBip,
  kPrevBip,
  kNextBipT,
  kPrevBipT,
  kAffectsBip,
  kAffectedByBip,
  kAffectsBipT,
  kAffectedByBipT,
  kInvalid
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
  kNextBip,
  kNextBipT,
  kAffects,
  kAffectsT,
  kAffectsBip,
  kAffectsBipT,
  kInvalid
};

enum class Attribute {
  kStmtNumber,
  kProcName,
  kVarName,
  kValue,
  kInvalid
};

/**
 * ScopeIndication is used by the PKBQueryReceiver when passing arguments to the DBManager.
 * This indicates whether the DBManager should utilize the scoped entity vectors (for left and right param) or just the DE itself.
 */
enum class ScopeIndication {
  kNoScope,
  kLeftScope, // i.e. only the left param is scoped, so use the entity vector for left param and DE for the right param
  kRightScope,
  kAllScope
};

DesignEntity GetDesignEntity(std::string reference);
RelRef GetRelRef(std::string reference);
Attribute GetAttribute(std::string attr_string);
Attribute GetDefaultAttribute(DesignEntity de);

const std::unordered_set<DesignEntity> stmt_design_entities_ = {
  DesignEntity::kRead,
  DesignEntity::kPrint,
  DesignEntity::kCall,
  DesignEntity::kWhile,
  DesignEntity::kIf,
  DesignEntity::kAssign
};

const std::unordered_set<DesignEntity> pattern_entities_ = {
  DesignEntity::kWhile,
  DesignEntity::kIf,
  DesignEntity::kAssign
};

const std::unordered_set<PKBRelRefs> preprocessed_rel_refs = {
  PKBRelRefs::kFollows,
  PKBRelRefs::kFollowsT,
  PKBRelRefs::kFollowedBy,
  PKBRelRefs::kFollowedByT,
  PKBRelRefs::kParent,
  PKBRelRefs::kParentT,
  PKBRelRefs::kChild,
  PKBRelRefs::kChildT,
  PKBRelRefs::kCalls,
  PKBRelRefs::kCalledBy,
  PKBRelRefs::kCallsT,
  PKBRelRefs::kCalledByT,
  PKBRelRefs::kUses,
  PKBRelRefs::kUsedBy,
  PKBRelRefs::kModifies,
  PKBRelRefs::kModifiedBy,
  PKBRelRefs::kNext,
  PKBRelRefs::kPrevious
};

const std::unordered_set<PKBRelRefs> second_param_is_stmt = {
  PKBRelRefs::kFollows,
  PKBRelRefs::kFollowsT,
  PKBRelRefs::kFollowedBy,
  PKBRelRefs::kFollowedByT,
  PKBRelRefs::kParent,
  PKBRelRefs::kParentT,
  PKBRelRefs::kChild,
  PKBRelRefs::kChildT,
  PKBRelRefs::kUsedBy,
  PKBRelRefs::kModifiedBy,
  PKBRelRefs::kNext,
  PKBRelRefs::kPrevious,
  PKBRelRefs::kNextT,
  PKBRelRefs::kPreviousT,
  PKBRelRefs::kAffects,
  PKBRelRefs::kAffectedBy,
  PKBRelRefs::kAffectsT,
  PKBRelRefs::kAffectedByT,
  PKBRelRefs::kNextBip,
  PKBRelRefs::kPrevBip,
  PKBRelRefs::kNextBipT,
  PKBRelRefs::kPrevBipT,
  PKBRelRefs::kAffectsBip,
  PKBRelRefs::kAffectedByBip,
  PKBRelRefs::kAffectsBipT,
  PKBRelRefs::kAffectedByBipT
};

const std::unordered_set<PKBRelRefs> second_param_is_var = {
  PKBRelRefs::kUses,
  PKBRelRefs::kModifies
};

const std::unordered_set<PKBRelRefs> second_param_is_proc = {
  PKBRelRefs::kCalls,
  PKBRelRefs::kCalledBy,
  PKBRelRefs::kCallsT,
  PKBRelRefs::kCalledByT
};

class Synonym {
 private:
  std::string name;
  DesignEntity type;
 public:
  Synonym() {};
  Synonym(std::string name, DesignEntity type) : name(name), type(type) {};
  std::string GetName() { return name; };
  DesignEntity GetType() { return type; };
  bool operator==(const Synonym& other) const;
};

// TODO: Need to refactor this to use the .cpp file instead, also the method name needs to be capitalized.
struct Clause {
  std::string left_hand_side;
  std::string right_hand_side;
  bool left_is_synonym;
  bool right_is_synonym;
  virtual std::vector<std::string> GetAllSynonymNamesOfClause() { return {}; };
  Synonym* first_synonym;
  Synonym* second_synonym;
  virtual std::string GetType() { return ""; };
  virtual bool IsEqual(Clause other_clause) { return true; };
  virtual ~Clause() = default;;
};

struct With : Clause {
  Attribute left_attribute = Attribute::kInvalid;
  Attribute right_attribute = Attribute::kInvalid;
  bool left_is_prog_line;
  bool right_is_prog_line;
  With() {};
  With(bool l_is_syn, bool r_is_syn, std::string lhs, std::string rhs, Attribute left_attr, Attribute right_attr,
       bool left_is_pl, bool right_is_pl) {
    left_is_synonym = l_is_syn;
    right_is_synonym = r_is_syn;
    left_hand_side = lhs;
    right_hand_side = rhs;
    left_attribute = left_attr;
    right_attribute = right_attr;
    left_is_prog_line = left_is_pl;
    right_is_prog_line = right_is_pl;
  };
  With(bool l_is_syn, bool r_is_syn, std::string lhs, std::string rhs, bool left_is_pl, bool right_is_pl) {
    left_is_synonym = l_is_syn;
    right_is_synonym = r_is_syn;
    left_hand_side = lhs;
    right_hand_side = rhs;
    left_is_prog_line = left_is_pl;
    right_is_prog_line = right_is_pl;
  };
  std::vector<std::string> GetAllSynonymNamesOfClause() {
    std::vector<std::string> v;
    if (left_is_synonym) v.push_back(left_hand_side);
    if (right_is_synonym) v.push_back(right_hand_side);
    return v;
  };
  std::string GetType() const { return typeid(this).name(); }
  bool IsEqual(Clause* other_obj) {
    if (this->GetType() == other_obj->GetType()) {
      With* obj = (With*) & other_obj;
      return (
              this->left_hand_side == obj->left_hand_side &&
              this->right_hand_side == obj->right_hand_side &&
              this->left_attribute == obj->left_attribute &&
              this->right_attribute == obj->right_attribute &&
              this->left_is_synonym == obj->left_is_synonym &&
              this->right_is_synonym == obj->right_is_synonym &&
              this->left_is_prog_line == obj->left_is_prog_line &&
              this->right_is_prog_line == obj->right_is_prog_line
              );
    } else {
      return false;
    }
  }
  bool HasEqualValue() {
    return this->left_hand_side == this->right_hand_side;
  }
  DesignEntity GetFirstSynonymType() {
    return this->first_synonym->GetType();
  }
  DesignEntity GetSecondSynonymType() {
    return this->second_synonym->GetType();
  }
};

struct SuchThat : Clause {
  RelRef rel_ref;
  SuchThat() {};
  SuchThat(std::string lhs, std::string rhs, RelRef rf, bool lhs_is_syn, bool rhs_is_syn) {
    left_hand_side = lhs;
    right_hand_side = rhs;
    rel_ref = rf;
    left_is_synonym = lhs_is_syn;
    right_is_synonym = rhs_is_syn;
  }
  std::vector<std::string> GetAllSynonymNamesOfClause() {
    std::vector<std::string> v;
    if (left_is_synonym) v.push_back(left_hand_side);
    if (right_is_synonym) v.push_back(right_hand_side);
    return v;
  };
  std::string GetType() const { return typeid(this).name(); }
  bool IsEqual(Clause* other_obj) {
    if (this->GetType() == other_obj->GetType()) {
      SuchThat* obj = (SuchThat*) & other_obj;
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
  DesignEntity pattern_type;
  bool is_exact = false;
  Pattern() { right_is_synonym = false; };
  Pattern(std::string lhs, std::string rhs, std::string assn_syn, bool lhs_is_syn, bool is_ext) {
    left_hand_side = lhs;
    right_hand_side = rhs;
    assign_synonym = assn_syn;
    left_is_synonym = lhs_is_syn;
    right_is_synonym = false;
    is_exact = is_ext;
  }
  Pattern(std::string lhs, std::string rhs, DesignEntity de, bool lhs_is_syn, bool is_ext) {
    left_hand_side = lhs;
    right_hand_side = rhs;
    pattern_type = de;
    left_is_synonym = lhs_is_syn;
    right_is_synonym = false;
    is_exact = is_ext;
  }
  std::vector<std::string> GetAllSynonymNamesOfClause() {
    std::vector<std::string> v;
    if (left_is_synonym) v.push_back(left_hand_side);
    v.push_back(first_synonym->GetName());
    return v;
  };
  std::string GetType() const { return typeid(this).name(); }
  bool IsEqual(Clause* other_obj) {
    if (this->GetType() == other_obj->GetType()) {
      Pattern* obj = (Pattern*) & other_obj;
      return (
          this->left_hand_side == obj->left_hand_side &&
              this->right_hand_side == obj->right_hand_side &&
              this->assign_synonym == obj->assign_synonym &&
              this->left_is_synonym == obj->left_is_synonym &&
              this->is_exact == obj->is_exact &&
              this->pattern_type == obj->pattern_type
      );
    } else {
      return false;
    }
  }
};

class Group {
 private:
  std::vector<Synonym*> target_synonyms;
  std::vector<Clause*> clauses;
  bool has_target_synonym = false;
 public:
  Group() {};
  Group(std::vector<Clause*> clauses, bool has_target_synonym) :
      has_target_synonym(has_target_synonym), clauses(clauses) {};
  Group(std::vector<Clause*> clauses, bool has_target_synonym, std::vector<Synonym*> target_synonyms) :
  has_target_synonym(has_target_synonym), clauses(clauses), target_synonyms(target_synonyms) {};
  void AddSynToTargetSyns(Synonym* s);
  void AddClauseToVector(Clause* clause);
  std::vector<Clause*> GetClauses();
  bool ContainsTargetSynonym();
  std::vector<Synonym*> GetTargetSynonyms();
  void UpdateHasTargetSynonymAttr();
  int GetGroupSize();
};

// functors for hashing purposes
struct SuchThatHash {
  std::size_t operator()(const SuchThat& cl) const {
    int hash = 17;
    hash = hash * 23 + std::hash<RelRef>()(cl.rel_ref);
    hash = hash * 23 + std::hash<std::string>()(cl.left_hand_side);
    hash = hash * 23 + std::hash<std::string>()(cl.right_hand_side);
    hash = hash * 23 + std::hash<bool>()(cl.left_is_synonym);
    hash = hash * 23 + std::hash<bool>()(cl.right_is_synonym);
    return hash;
  }
};

struct PatternHash {
  std::size_t operator()(const Pattern& cl) const {
    int hash = 17;
    hash = hash * 23 + std::hash<std::string>()(cl.left_hand_side);
    hash = hash * 23 + std::hash<std::string>()(cl.right_hand_side);
    hash = hash * 23 + std::hash<bool>()(cl.left_is_synonym);
    hash = hash * 23 + std::hash<bool>()(cl.right_is_synonym);
    hash = hash * 23 + std::hash<bool>()(cl.is_exact);
    hash = hash * 23 + std::hash<std::string>()(cl.assign_synonym);
    hash = hash * 23 + std::hash<DesignEntity>()(cl.pattern_type);
    return hash;
  }
};

struct WithHash {
  std::size_t operator()(const With& cl) const {
    int hash = 17;
    hash = hash * 23 + std::hash<std::string>()(cl.left_hand_side);
    hash = hash * 23 + std::hash<std::string>()(cl.right_hand_side);
    hash = hash * 23 + std::hash<Attribute>()(cl.left_attribute);
    hash = hash * 23 + std::hash<Attribute>()(cl.right_attribute);
    hash = hash * 23 + std::hash<bool>()(cl.left_is_synonym);
    hash = hash * 23 + std::hash<bool>()(cl.right_is_synonym);
    hash = hash * 23 + std::hash<bool>()(cl.left_is_prog_line);
    hash = hash * 23 + std::hash<bool>()(cl.right_is_prog_line);
    return hash;
  }
};

// functors for checking for equivalence
struct SuchThatComparator {
  bool operator()(const SuchThat& c1, const SuchThat& c2) const {
    return c1.rel_ref == c2.rel_ref && c1.left_hand_side == c2.left_hand_side &&
    c1.right_hand_side == c2.right_hand_side && c1.left_is_synonym == c2.left_is_synonym &&
    c1.right_is_synonym == c2.right_is_synonym;
  }
};

struct PatternComparator {
  bool operator()(const Pattern& c1, const Pattern& c2) const {
    return c1.pattern_type == c2.pattern_type && c1.left_hand_side == c2.left_hand_side &&
    c1.right_hand_side == c2.right_hand_side && c1.left_is_synonym == c2.left_is_synonym &&
    c1.right_is_synonym == c2.right_is_synonym && c1.is_exact == c2.is_exact && c1.assign_synonym == c2.assign_synonym;
  }
};

struct WithComparator {
  bool operator()(const With& c1, const With& c2) const {
    return c1.left_hand_side == c2.left_hand_side && c1.right_hand_side == c2.right_hand_side &&
    c1.left_is_synonym == c2.left_is_synonym && c1.right_is_synonym == c2.right_is_synonym &&
    c1.left_attribute == c2.left_attribute && c1.right_attribute == c2.right_attribute &&
    c1.left_is_prog_line == c2.left_is_prog_line && c1.right_is_prog_line == c2.right_is_prog_line;
  }
};

#endif //AUTOTESTER_TYPES_H
