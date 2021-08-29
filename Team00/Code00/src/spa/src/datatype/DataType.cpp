/**
 * This body file contains the different function implementation used for datatype.
 */

#include <string>
#include <stdexcept>
#include "DataType.h"

/**
 * This @code{StatementNumber} constructor check if the statement number input is valid,
 *   and stores as a @code{StatementNumber} object.
 */
StatementNumber::StatementNumber(int sn) {
    if (sn < 1)
        std::invalid_argument("Statement Number should not be negative.");
    num = sn;
}

/**
 * This @code{LineNumber} constructor check if the line number input is valid,
 *   and stores as a @code{LineNumber} object.
 */
LineNumber::LineNumber(int ln) {
    if (ln < 1)
        std::invalid_argument("Line Number should not be negative.");
    num = ln;
}

/**
 * This @code{ProcedureName} constructor check if the procedure name is valid,
 *   and stores as a @code{ProcedureName} object.
 */
ProcedureName::ProcedureName(std::string pName) {
    //TODO: validate name syntax
    name = pName;
}

/**
 * This @code{VariableName} constructor check if the variable name is valid,
 *   and stores as a @code{VariableName} object.
 */
VariableName::VariableName(std::string vName) {
    //TODO: validate name syntax
    name = vName;
}

/**
 * This @code{Constant} constructor check if the constant received is valid (valid = integer),
 *   and stores as a @code{Constant} object.
 */
Constant::Constant(std::string constant) {
    //TODO: check if constant is valid.
    digits = stoi(constant);
}