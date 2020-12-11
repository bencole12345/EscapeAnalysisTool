#ifndef ESCAPEANALYSISTOOL_CSVWRITER
#define ESCAPEANALYSISTOOL_CSVWRITER

#include <fstream>
#include <string>
#include <vector>

namespace EscapeAnalysisTool {

class CSVWriter {
private:
    std::ofstream outputStream;

public:
    explicit CSVWriter(std::string& outputFile);
    ~CSVWriter();
    void addEntry(const std::string& fileName, int numFunctions, int numEscapingFunctions);
};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_CSVWRITER
