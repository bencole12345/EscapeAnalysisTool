#include "CSVWriter.h"

#include <ostream>

namespace EscapeAnalysisTool {

CSVWriter::CSVWriter(std::string& outputFile)
        : outputStream()
{
    outputStream.open(outputFile);
    outputStream << "FileName,FunctionName,NumStackAllocations,NumEscapingStackAllocations" << std::endl;
}

CSVWriter::~CSVWriter()
{
    outputStream.close();
}

void CSVWriter::addEntry(
        const std::string& fileName,
        const std::string& functionName,
        int numStackAllocations,
        int numEscapingStackAllocations)
{
    outputStream << fileName << ","
                 << functionName << ","
                 << numStackAllocations << ","
                 << numEscapingStackAllocations << std::endl;
}

} // namespace EscapeAnalysisTool
