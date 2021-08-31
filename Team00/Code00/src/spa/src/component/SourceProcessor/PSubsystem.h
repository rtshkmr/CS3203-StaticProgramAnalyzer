#ifndef AUTOTESTER_PSUBSYSTEM_H
#define AUTOTESTER_PSUBSYSTEM_H

#include <string>
#include <datatype/Deliverable.h>

/**
 * This namespace provides a scope for PSubsystem related declarations.
 */
namespace psub {
    /**
     * This class handles the interactions between sub-components that help parse the source code. It contains the
     * data structures that accumulate (AST, EntityTables, RelationshipTables, Helper Stacks that keep track of tokens
     * and determine how the Deliverables are being modified). It receives a single Source Statement from the Parser and
     * initiates a pipeline for processing it: the statement is tokenized then its syntax is validated, entity nodes are
     * created and added to the AST, relationships are added.
     */
    class PSubsystem {
    private:
        Deliverable deliverable_;
        // attributes
    public:
        PSubsystem() = default;

        void InitDataStructures();

        void ProcessStatement(std::string statement);

        Deliverable GetDeliverables();
    };
}

#endif //AUTOTESTER_PSUBSYSTEM_H
