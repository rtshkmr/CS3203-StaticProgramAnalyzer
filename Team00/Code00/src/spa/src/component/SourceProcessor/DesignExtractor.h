#ifndef AUTOTESTER_DESIGNEXTRACTOR_H
#define AUTOTESTER_DESIGNEXTRACTOR_H

/**
 * This namespace provides a scope for DesignExtractor related declarations.
 */
namespace de {
    /**
     * This class encapsulates the extraction of design relationships from the data structures available in the Source
     * Processor.
     */
    class DesignExtractor {
    private:
        // attributes
    public:
        DesignExtractor() = default;

        void extractDesignAbstractions();
    };
}

#endif //AUTOTESTER_DESIGNEXTRACTOR_H
