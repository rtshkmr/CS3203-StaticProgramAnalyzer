#ifndef AUTOTESTER_PARSER_H
#define AUTOTESTER_PARSER_H

#include <string>
#include <datatype/Deliverable.h>

/**
 * This namespace provides a scope for Parser related declarations.
 */
namespace par {
    /**
     * Parser is a facade that takes in the source program from the SourceProcessor, checks the syntax, creates the
     * necessary data structures and sends these Deliverables back.
     */
    class Parser {
    private:
        Deliverable deliverable_;
    public:
        Parser() = default;

        void Parse(std::string file_name);

        Deliverable* GetDeliverables();
    };
}

#endif //AUTOTESTER_PARSER_H
