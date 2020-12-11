#ifndef ESCAPEANALYSISTOOL_FUNCTIONSUMMARY
#define ESCAPEANALYSISTOOL_FUNCTIONSUMMARY

#include <string>

namespace EscapeAnalysisTool {

struct FunctionSummary {
    const std::string& fileName;
    const std::string& functionName;
    int totalStackAllocations;
    int totalEscapingStackAllocations;
    uint64_t totalAllocatedStackMemory;
    uint64_t totalAllocatedEscapingStackMemory;
};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_FUNCTIONSUMMARY
