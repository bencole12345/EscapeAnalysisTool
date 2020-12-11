#include "CSVWriter.h"

#include <ostream>

namespace EscapeAnalysisTool {

CSVWriter::CSVWriter(std::string& outputFile)
        : outputStream()
{
    outputStream.open(outputFile);
    outputStream << "FileName,NumFunctions,NumEscapingFunctions" << std::endl;
}

CSVWriter::~CSVWriter()
{
    outputStream.close();
}

void CSVWriter::addEntry(const std::string& fileName, int numFunctions, int numEscapingFunctions)
{
    outputStream << fileName << "," << numFunctions << "," << numEscapingFunctions << std::endl;
}

} // namespace EscapeAnalysisTool
