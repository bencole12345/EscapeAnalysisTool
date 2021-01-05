#ifndef ESCAPEANALYSISTOOL_FUNCTIONSUMMARY
#define ESCAPEANALYSISTOOL_FUNCTIONSUMMARY

#include <string>

namespace EscapeAnalysisTool {

/**
 * Contains a summary of a function's stack memory usage.
 * 
 * This is the primary output of the escape analysis process for a function.
 */
struct FunctionSummary {

    /**
     * The name of the function.
     */
    const std::string& functionName;

    /**
     * The number of instructions in the function.
     */
    unsigned int numInstructions;

    /**
     * The number of static alloca calls in the function.
     */
    unsigned int numAllocaInvocations;

    /**
     * The number of alloca calls whose returned pointers have been deemed to
     * be possibly-escaping.
     */
    unsigned int numEscapingAllocaInvocations;

    /**
     * The number of alloca calls whose sizes are not compile-time constants.
     */
    unsigned int numDynamicallySizedAllocaInvocations;

    /**
     * The number of alloca calls whose sizes are not compile-time constants
     * and whose returned pointers have been deemed to be possibly-escaping.
     */
    unsigned int numDynamicallySizedEscapingAllocaInvocations;

    /**
     * The total amount of memory allocated in constant-sized alloca calls.
     */
    uint64_t totalStaticallyAllocatedStackMemory;

    /**
     * The total amount of memory allocated in constant-sized alloca calls
     * to which pointers may escape.
     */
    uint64_t totalStaticallyAllocatedEscapingStackMemory;
};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_FUNCTIONSUMMARY
