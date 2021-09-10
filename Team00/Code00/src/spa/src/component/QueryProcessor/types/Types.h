#ifndef AUTOTESTER_TYPES_H
#define AUTOTESTER_TYPES_H

#include <string>
#include <vector>
#include <datatype/DataType.h>

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
  // TODO: Throw an error if this line is reached.
  return DesignEntity::kInvalid;
}

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
};

class Synonym {
  private:
    std::string name;
    DesignEntity type;
  public:
    Synonym(std::string name, DesignEntity type): name(name), type(type) {};
    std::string GetName() { return name; };
    DesignEntity GetType() { return type; };
};

class Clause {
  public:
  std::string left_hand_side;
  std::string right_hand_side;
};

class SuchThat : public Clause {
  public:
    RelRef rel_ref;
    bool left_is_synonym;
    bool right_is_synonym;
};

class Pattern : public Clause {
  public:
    std::string assign_synonym;
    bool left_is_synonym;
    bool is_exact;
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
