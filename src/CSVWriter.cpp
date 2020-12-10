#include "CSVWriter.h"

#include <ostream>

namespace EscapeAnalysisTool {

CSVWriter::CSVWriter(std::ostream& outputStream)
        : outputStream(outputStream)
{
    outputStream << "FileName,NumFunctions,NumEscapingFunctions" << std::endl;
}

void CSVWriter::addEntry(const std::string& fileName, int numFunctions, int numEscapingFunctions)
{
    outputStream << fileName << "," << numFunctions << "," << numEscapingFunctions << std::endl;
}

} // namespace EscapeAnalysisTool
