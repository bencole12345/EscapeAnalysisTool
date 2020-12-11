#include "EscapeAnalyser.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <llvm/Analysis/CaptureTracking.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "CSVWriter.h"

#define MAX_USES_TO_EXPLORE 10000

namespace EscapeAnalysisTool {

EscapeAnalyser::EscapeAnalyser(llvm::LLVMContext& context, llvm::SMDiagnostic& err, CSVWriter& writer, bool verbose)
    : context(context), err(err), writer(writer), verbose(verbose)
{
}

void EscapeAnalyser::processFile(const std::string& filePath)
{
    std::unique_ptr<llvm::Module> module(llvm::parseIRFile(filePath, err, context));
    if (!module) {
        std::cerr << "Bad input file: " << filePath;
        exit(2);
    }

    for (const llvm::Function& function : module->functions()) {
        processFunction(function, filePath);
    }
}

void EscapeAnalyser::processFunction(const llvm::Function& function, const std::string& filePath)
{
    int numStackAllocationsTotal = 0;
    int numEscapingStackAllocations = 0;

    for (const llvm::BasicBlock& basicBlock : function.getBasicBlockList()) {
        for (const llvm::Instruction& instruction : basicBlock.getInstList()) {

            // We're only interested in stack allocations
            if (instruction.getOpcode() != llvm::Instruction::Alloca)
                continue;

            numStackAllocationsTotal++;

            // Run the analysis
            bool returnCaptures = true;
            bool storeCaptures = true;
            if (llvm::PointerMayBeCaptured(&instruction, returnCaptures, storeCaptures, MAX_USES_TO_EXPLORE)) {

                // We found a capture
                numEscapingStackAllocations++;

                if (verbose) {
                    std::cerr << "Capture found\n    function: " << llvm::demangle(function.getName()) << "\n    dump: ";
                    instruction.dump();
                }
            }
        }
    }

    // Write the results to the CSV file. We need to replace the commas in the
    // functions' type lists with semicolons because it's a CSV file and that
    // will mess up the parsing.
    std::string functionName = llvm::demangle(function.getName());
    std::replace(functionName.begin(), functionName.end(), ',', ';');
    writer.addEntry(filePath, functionName, numStackAllocationsTotal, numEscapingStackAllocations);
}

} // namespace EscapeAnalysisTool
