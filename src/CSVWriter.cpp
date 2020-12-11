#include "CSVWriter.h"

#include <ostream>

#include "FunctionSummary.h"

namespace EscapeAnalysisTool {

CSVWriter::CSVWriter(std::string& outputFile)
        : outputStream()
{
    outputStream.open(outputFile);
    outputStream << "FileName;FunctionName;NumStackAllocations;NumEscapingStackAllocations;"
                 << "TotalAllocatedStackMemory;TotalEscapingAllocateStackMemory" << std::endl;
}

CSVWriter::~CSVWriter()
{
    outputStream.close();
}

void CSVWriter::addEntry(const FunctionSummary& summary)
{
    outputStream << summary.fileName << ";"
                 << summary.functionName << ";"
                 << summary.totalStackAllocations << ";"
                 << summary.totalEscapingStackAllocations << ";"
                 << summary.totalAllocatedStackMemory << ";"
                 << summary.totalAllocatedEscapingStackMemory
                 << std::endl;
}

} // namespace EscapeAnalysisTool
