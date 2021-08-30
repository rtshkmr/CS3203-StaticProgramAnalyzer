/**
 * This header file contains the different data types used for this program.
 * Any data types used in this program should be immutable by default.
 */

#include "string"

#ifndef AUTOTESTER_DATATYPE_H
#define AUTOTESTER_DATATYPE_H

/**
 * StatementNumber refers to the numbering of statements (program counter).
 * Note: The difference between StatementNumber and LineNumber is that a single line could have multiple
 *     statements, one statement, or partial statement.
 */
class StatementNumber {
private:
    int num;

public:
    StatementNumber(int sn);
    int getNum();
};

/**
 * LineNumber refers to the numbering of lines based on the input SIMPLE source code.
 * Note: The difference between StatementNumber and LineNumber is that a single line could have multiple
 *     statements, one statement, or partial statement.
 */
class LineNumber {
private:
    int num;

public:
    LineNumber(int ln);
    int getNum();
};

/**
 * ProcedureName refers to the name for a procedure.
 * This object checks and ensure that the received procedure name is correct according to the lexical tokens
 */
class ProcedureName {
private:
    std::string name;

public:
    ProcedureName(std::string pName);
    std::string getName();
};

/**
 * VariableName refers to the name for a variable.
 * This object checks and ensure that the received variable name is correct according to the lexical tokens
 */
class VariableName {
private:
    std::string name;

public:
    VariableName(std::string vName);
    std::string getName();
};

/**
 * Constant refers to a constant encountered in the program.
 * This object checks and ensure that the received constants is correct according to the lexical tokens,
 *   and based on the program rule limiting the number of digits within a constant.
 * Side note: constants does not have a name to identify them.
 */
class Constant {
private:
    int digits;

public:
    Constant(std::string constant);
    int get();
};

#endif //AUTOTESTER_DATATYPE_H
