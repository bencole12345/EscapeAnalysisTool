#include "EscapeAnalyser.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <llvm/Analysis/CaptureTracking.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "CSVWriter.h"
#include "FunctionSummary.h"

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
        processFunction(function, module);
    }
}

void EscapeAnalyser::processFunction(const llvm::Function& function, std::unique_ptr<llvm::Module>& module)
{
    int numStackAllocationsTotal = 0;
    int numEscapingStackAllocations = 0;

    uint64_t stackAllocationsTotalSize = 0;
    uint64_t escapingStackAllocationsTotalSize = 0;

    llvm::DataLayout dataLayout(module.get());

    for (const llvm::BasicBlock& basicBlock : function.getBasicBlockList()) {
        for (const llvm::Instruction& instruction : basicBlock.getInstList()) {

            // We're only interested in stack allocations
            if (instruction.getOpcode() != llvm::Instruction::Alloca)
                continue;

            // Extract the size of the allocatoin
            const auto& allocation = reinterpret_cast<const llvm::AllocaInst&>(instruction);
            uint64_t allocationSize = dataLayout.getTypeAllocSize(allocation.getAllocatedType()).getFixedSize();
            // TODO: Handle non-constant allocation sizes (dynamically-sized)

            numStackAllocationsTotal++;
            stackAllocationsTotalSize += allocationSize;

            // Run the analysis
            bool returnCaptures = true;
            bool storeCaptures = true;
            if (llvm::PointerMayBeCaptured(&allocation, returnCaptures, storeCaptures, MAX_USES_TO_EXPLORE)) {

                // We found a capture
                numEscapingStackAllocations++;
                escapingStackAllocationsTotalSize += allocationSize;

                if (verbose) {
                    std::cerr << "Capture found\n    function: " << llvm::demangle(function.getName().data()) << "\n    dump: ";
                    instruction.dump();
                }
            }
        }
    }

    std::string functionName = llvm::demangle(function.getName().data());
    writer.addEntry(FunctionSummary{
            .fileName = module->getSourceFileName(),
            .functionName = functionName,
            .totalStackAllocations = numStackAllocationsTotal,
            .totalEscapingStackAllocations = numEscapingStackAllocations,
            .totalAllocatedStackMemory = stackAllocationsTotalSize,
            .totalAllocatedEscapingStackMemory = escapingStackAllocationsTotalSize
    });
}

} // namespace EscapeAnalysisTool
