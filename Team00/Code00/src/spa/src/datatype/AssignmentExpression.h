/**
 * This header file contains the object and method signatures of @code{AssignmentExpression}.
 */

#ifndef AUTOTESTER_ASSIGNMENTEXPRESSION_H
#define AUTOTESTER_ASSIGNMENTEXPRESSION_H

#include <string>

class AssignmentExpression {
private:
    std::string expression;
    std::string parseInfixToPostfix(std::string infix);
public:
    AssignmentExpression(std::string infix);
    bool checkExist(std::string pattern);
    bool checkExact(std::string pattern);
    bool checkBefore(std::string pattern);
    bool checkAfter(std::string pattern);
};

#endif //AUTOTESTER_ASSIGNMENTEXPRESSION_H
