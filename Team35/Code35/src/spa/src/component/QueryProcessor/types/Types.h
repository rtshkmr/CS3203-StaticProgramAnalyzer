#ifndef AUTOTESTER_TYPES_H
#define AUTOTESTER_TYPES_H

#include <string>
#include <vector>
#include <array>
#include <set>
#include <datatype/DataType.h>
//#include <component/QueryProcessor/types/QueryEvaluatorTable.h>
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
  kWildcard = 11,
  kProgLine = 12
};

const std::array<DesignEntity, 12> all_design_entities = {
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
    DesignEntity:: kProgLine,
};

DesignEntity GetDesignEntity(std::string reference);

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
  kAffectedByT
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

RelRef GetRelRef(std::string reference);

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

Attribute GetAttribute(std::string attr_string);

struct QueryInfo {
  bool all_boolean_true;
  // std::vector<*QueryEvaluatorTable> table_list;
};

class Synonym {
 private:
  std::string name;
  DesignEntity type;
  Attribute return_attribute = Attribute::kStmtNumber;
 public:
  Synonym() {};
  Synonym(std::string name, DesignEntity type) : name(name), type(type) {};
  Synonym(std::string name, DesignEntity type, Attribute attr) : name(name), type(type), return_attribute(attr) {};
  std::string GetName() { return name; };
  DesignEntity GetType() { return type; };
  Attribute GetAttribute() { return return_attribute; };
  void SetAttribute(Attribute attr) { return_attribute = attr; };
  bool operator==(const Synonym& other) const;
};

struct Clause {
  std::string left_hand_side;
  std::string right_hand_side;
  virtual std::vector<std::string> GetAllSynonymNamesOfClause() { return {}; };
  Synonym* first_synonym;
  Synonym* second_synonym;
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
  std::vector<std::string> GetAllSynonymNamesOfClause() {
    std::vector<std::string> v;
    if (left_is_synonym) v.push_back(left_hand_side);
    if (right_is_synonym) v.push_back(right_hand_side);
    return v;
  };
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
  DesignEntity pattern_type;
  bool left_is_synonym;
  bool is_exact = false;
  Pattern() {};
  Pattern(std::string lhs, std::string rhs, std::string assn_syn, bool lhs_is_syn, bool is_ext) {
    left_hand_side = lhs;
    right_hand_side = rhs;
    assign_synonym = assn_syn;
    left_is_synonym = lhs_is_syn;
    is_exact = is_ext;
  }
  Pattern(std::string lhs, std::string rhs, DesignEntity de, bool lhs_is_syn, bool is_ext) {
    left_hand_side = lhs;
    right_hand_side = rhs;
    pattern_type = de;
    left_is_synonym = lhs_is_syn;
    is_exact = is_ext;
  }
  std::vector<std::string> GetAllSynonymNamesOfClause() {
    std::vector<std::string> v;
    if (left_is_synonym) v.push_back(left_hand_side);
    v.push_back(first_synonym->GetName());
    return v;
  };
  std::string getType() const { return typeid(this).name(); }
  bool isEqual(Clause* toObj) {
    if (this->getType() == toObj->getType()) {
      Pattern* obj = (Pattern*) & toObj;
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
};

#endif //AUTOTESTER_TYPES_H
