#include "CSVWriter.h"

#include <ostream>

#include "FunctionSummary.h"

namespace {

const char * headings[] = {
    "FunctionName",
    "NumAllocaInvocations",
    "NumEscapingAllocaInvocations",
    "NumDynamicallySizedAllocaInvocations",
    "NumDynamicallySizedEscapingAllocaInvocations",
    "TotalStaticallyAllocatedStackMemory",
    "TotalStaticallyAllocatedEscapingStackMemory"
};

const char * separator = ";";

} // anonymous namespace

namespace EscapeAnalysisTool {

CSVWriter::CSVWriter(std::string& outputFile)
        : outputStream()
{
    outputStream.open(outputFile);

    // Write each heading to the output file
    size_t num_headings = sizeof(headings) / sizeof(*headings);
    for (size_t i = 0; i < num_headings; i++) {
        outputStream << headings[i];

        // If it's the last one then end the line
        if (i == num_headings - 1) {
            outputStream << std::endl;
        }

        // Otherwise write the separator
        else {
            outputStream << separator;
        }
    }
}

CSVWriter::~CSVWriter()
{
    outputStream.close();
}

void CSVWriter::addEntry(const FunctionSummary&& summary)
{
    outputStream << summary.functionName << separator
                 << summary.numAllocaInvocations << separator
                 << summary.numEscapingAllocaInvocations << separator
                 << summary.numDynamicallySizedAllocaInvocations << separator
                 << summary.numDynamicallySizedEscapingAllocaInvocations << separator
                 << summary.totalStaticallyAllocatedStackMemory << separator
                 << summary.totalStaticallyAllocatedEscapingStackMemory
                 << std::endl;
}

} // namespace EscapeAnalysisTool
