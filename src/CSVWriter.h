#ifndef ESCAPEANALYSISTOOL_CSVWRITER
#define ESCAPEANALYSISTOOL_CSVWRITER

#include <fstream>
#include <string>
#include <vector>

namespace EscapeAnalysisTool {

/**
 * Encapsulates writing rows to a CSV file.
 */
class CSVWriter {
private:
    std::ofstream outputStream;

public:
    explicit CSVWriter(std::string& outputFile);
    ~CSVWriter();

    /**
     * Adds a row to the CSV file.
     *
     * @param fileName The name of the file being analysed
     * @param functionName The function we are reporting
     * @param numStackAllocations The number of (static) alloca calls
     * @param numEscapingStackAllocations The number of alloca calls whose returned pointers
     *                                    may escape the function
     */
    void addEntry(
            const std::string& fileName,
            const std::string& functionName,
            int numStackAllocations,
            int numEscapingStackAllocations);
};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_CSVWRITER
