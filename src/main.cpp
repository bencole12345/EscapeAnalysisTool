#include <iostream>
#include <vector>

#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>

#include "ArgumentParser.h"
#include "EscapeAnalyser.h"
#include "CSVWriter.h"

using namespace EscapeAnalysisTool;

int main(int argc, char **argv)
{
    // Read args to determine which files to process
    std::vector<std::string> filesToProcess;
    parseArguments(&filesToProcess, argc, argv);

    // Create our CSV writer
    CSVWriter writer(std::cout);

    // Set up the escape analysis tool
    llvm::LLVMContext context;
    llvm::SMDiagnostic err;
    EscapeAnalyser analyser(context, err);

    // Analyse each file
    for (const std::string& path : filesToProcess) {
        analyser.processFile(path, writer);
    }

    return 0;
}
