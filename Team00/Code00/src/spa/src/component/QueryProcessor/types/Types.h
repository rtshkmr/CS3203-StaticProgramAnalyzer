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

static DesignEntity getDesignEntity(std::string reference) {
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
    std::string getName() { return name; };
    DesignEntity getType() { return type; };
};

class Clause {
  public:
  std::string leftHandSide;
  std::string rightHandSide;
};

class SuchThat : Clause {
  public:
    RelRef relRef;
    bool leftIsSynonym;
    bool rightIsSynonym;
};

class Pattern : Clause {
  public:
    std::string assignSynonym;
    bool leftIsSynonym;
    bool isExact;
};

class Group {
  private:
    std::vector<Clause> clauses;
    bool hasTargetSynonym;
  public:
    Group(std::vector<Clause> clauses, bool hasTargetSynonym) : hasTargetSynonym(hasTargetSynonym), clauses(clauses) {};
    bool addClauseToVector(Clause clause);
    std::vector<Clause> getClauses();
    bool containsTargetSynonym();
};


#endif //AUTOTESTER_TYPES_H
