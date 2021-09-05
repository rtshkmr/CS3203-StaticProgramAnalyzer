#ifndef AUTOTESTER_TYPES_H
#define AUTOTESTER_TYPES_H

#include <string>

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
  kProcedure
};

class Synonym {
  private:
    std::string name;
    DesignEntity type;
  public:
    std::string getName() { return name; };
    DesignEntity getType() { return type; };
};

class Group {
  // TODO
};

#endif //AUTOTESTER_TYPES_H
