#ifndef AUTOTESTER_IDENTIFIER_H
#define AUTOTESTER_IDENTIFIER_H

#include <string>

/**
 * This is a return type used to encapsulate the status of a method call. This class will be redundant if using
 * exception classes to handle errors anyway.
 */
enum class Identifier {
    kSuccess,
    kFailure    // can add more meaningful failure descriptors
};

#endif //AUTOTESTER_IDENTIFIER_H
