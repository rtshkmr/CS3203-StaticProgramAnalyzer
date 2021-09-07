#ifndef AUTOTESTER_TYPES_H
#define AUTOTESTER_TYPES_H

#include <string>
#include <vector>

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
};

class Synonym {
  private:
    std::string name;
    DesignEntity type;
  public:
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
