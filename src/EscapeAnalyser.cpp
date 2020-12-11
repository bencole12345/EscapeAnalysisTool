#include "EscapeAnalyser.h"

#include <iostream>

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

#define DEBUG_PRINT 1

namespace EscapeAnalysisTool {

EscapeAnalyser::EscapeAnalyser(llvm::LLVMContext& context, llvm::SMDiagnostic& err)
    : context(context), err(err)
{
}

void EscapeAnalyser::processFile(const std::string& filePath, CSVWriter& writer)
{
    // TODO: Handle malformed file (at the moment it just exits)
    std::unique_ptr<llvm::Module> module(llvm::parseIRFile(filePath, err, context));

    int functionsCount = 0;
    int unsafeFunctionsCount = 0;

    for (const llvm::Function& function : module->functions()) {
        functionsCount++;
        if (pointerToStackAllocationMayEscape(function))
            unsafeFunctionsCount++;
    }

    writer.addEntry(filePath, functionsCount, unsafeFunctionsCount);
}

bool EscapeAnalyser::pointerToStackAllocationMayEscape(const llvm::Function& function)
{
    for (const llvm::BasicBlock& basicBlock : function.getBasicBlockList()) {
        for (const llvm::Instruction& instruction : basicBlock.getInstList()) {

            // We're only interested in stack allocations
            if (instruction.getOpcode() != llvm::Instruction::Alloca)
                continue;

            // Run the analysis
            if (llvm::PointerMayBeCaptured(&instruction, true, false, MAX_USES_TO_EXPLORE)) {

                // We found a capture

#if DEBUG_PRINT
                std::cerr << "Capture found\n    function: " << llvm::demangle(function.getName()) << "\n    dump: ";
                instruction.dump();
#endif
                return true;
            }

        }
    }

    // We failed to find a captured pointer so we can safely return false
    return false;
}

} // namespace EscapeAnalysisTool
