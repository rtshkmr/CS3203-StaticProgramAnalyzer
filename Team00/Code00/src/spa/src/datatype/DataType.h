/**
 * This header file contains the different data types used for this program.
 */

#ifndef AUTOTESTER_DATATYPE_H
#define AUTOTESTER_DATATYPE_H

/**
 * StatementNumber refers to the numbering of statements (program counter).
 * Note: The difference between @code{StatementNumber} and @code{LineNumber} is that a single line could have multiple
 *     statements, one statement, or partial statement.
 */
class StatementNumber {
public:
    int num;
    StatementNumber(int sn);
};

/**
 * LineNumber refers to the numbering of lines based on the input SIMPLE source code.
 * Note: The difference between @code{StatementNumber} and @code{LineNumber} is that a single line could have multiple
 *     statements, one statement, or partial statement.
 */
class LineNumber {
public:
    int num;
    LineNumber(int ln);
};

/**
 * @code{ProcedureName} refers to the name for a procedure.
 * This object checks and ensure that the received procedure name is correct according to the lexical tokens
 */
class ProcedureName {
private:
    std::string name;

public:
    ProcedureName(std::string pName);
};

/**
 * @code{VariableName} refers to the name for a variable.
 * This object checks and ensure that the received variable name is correct according to the lexical tokens
 */
class VariableName {
private:
    std::string name;

public:
    VariableName(std::string vName);
};

/**
 * @code{Constant} refers to a constant encountered in the program.
 * This object checks and ensure that the received constants is correct according to the lexical tokens,
 *   and based on the program rule limiting the number of digits within a constant.
 * Side note: constants does not have a name to identify them.
 */
class Constant {
private:
    int digits;
public:
    Constant(std::string constant);
};

#endif //AUTOTESTER_DATATYPE_H
