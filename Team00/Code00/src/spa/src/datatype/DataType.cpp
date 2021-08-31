/**
 * This body file contains the different function implementation used for datatype.
 */

#include <string>
#include <stdexcept>
#include "DataType.h"

/**
 * This StatementNumber constructor check if the statement number input is valid,
 *   and stores as a StatementNumber object.
 * @param sn [NOT NULL] The statement number
 * @throws std::invalid_argument when a negative integer is passed in.
 */
StatementNumber::StatementNumber(int sn) {
    if (sn < 1)
        throw std::invalid_argument("Statement Number should not be negative.");
    num = sn;
}

/**
 * Gets the statement number.
 * @return [Not Null] The statement number
 */
int StatementNumber::getNum() {
    return num;
}

/**
 * This LineNumber constructor check if the line number input is valid,
 *   and stores as a LineNumber object.
 * @param ln [NOT NULL] The line number
 * @throws std::invalid_argument when a negative integer is passed in.
 */
LineNumber::LineNumber(int ln) {
    if (ln < 1)
        throw std::invalid_argument("Line Number should not be negative.");
    num = ln;
}

/**
 * Gets the line number.
 * @return [Not Null] The line number
 */
int LineNumber::getNum() {
    return num;
}

/**
 * This ProcedureName constructor check if the procedure name is valid,
 *   and stores as a ProcedureName object.
 * @param pName [NOT NULL] The procedure name (in string) as extracted from SIMPLE program
 * @throws std::invalid_argument when an invalid procedure name is passed in.
 */
ProcedureName::ProcedureName(std::string pName) {
    //TODO: validate name syntax
    name = pName;
}

/**
 * Gets the name of the procedure.
 * @return [Not Null] The procedure name
 */
std::string ProcedureName::getName() {
    return name;
}

/**
 * This VariableName constructor check if the variable name is valid,
 *   and stores as a VariableName object.
 * @param vName [NOT NULL] The variable name (in string) as extracted from SIMPLE program
 * @throws std::invalid_argument when an invalid variable name is passed in.
 */
VariableName::VariableName(std::string vName) {
    //TODO: validate name syntax
    name = vName;
}

/**
 * Gets the name of the variable.
 * @return [Not Null] The variable name
 */
std::string VariableName::getName() {
    return name;
}

/**
 * This Constant constructor check if the constant received is valid (valid = integer),
 *   and stores as a Constant object.
 * @param constant [NOT NULL] The constant (in string) as extracted from SIMPLE program
 * @throws std::invalid_argument when a non-integer in passed in.
 */
Constant::Constant(std::string constant) {
    //TODO: check if constant is valid.
    digits = stoi(constant);
}

/**
 * Gets the constant value.
 * @return [Not Null] The constant value
 */
int Constant::get() {
    return digits;
}
