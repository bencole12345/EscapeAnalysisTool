#ifndef ESCAPEANALYSISTOOL_CSVWRITER
#define ESCAPEANALYSISTOOL_CSVWRITER

#include <string>
#include <vector>
#include <fstream>

namespace EscapeAnalysisTool {

class CSVWriter {
private:
    std::ostream& outputStream;

public:
    explicit CSVWriter(std::ostream& outputStream);
    void addEntry(const std::string& fileName, int numFunctions, int numEscapingFunctions);
};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_CSVWRITER
