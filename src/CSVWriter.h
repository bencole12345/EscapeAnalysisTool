#ifndef ESCAPEANALYSISTOOL_CSVWRITER
#define ESCAPEANALYSISTOOL_CSVWRITER

#include <fstream>
#include <string>

#include "FunctionSummary.h"

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
     */
    void addEntry(const FunctionSummary& summary);
};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_CSVWRITER
