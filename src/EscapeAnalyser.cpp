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
#include <llvm/IR/Type.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TypeSize.h>

#include "CSVWriter.h"
#include "FunctionSummary.h"

#define MAX_USES_TO_EXPLORE 10000

namespace {

uint64_t getAllocationStaticSize(const llvm::AllocaInst& allocation, const llvm::DataLayout& dataLayout)
{
    llvm::Type *allocatedType = allocation.getAllocatedType();
    llvm::TypeSize typeSize = dataLayout.getTypeAllocSize(allocatedType);
    return typeSize.getFixedSize();
}

} // anonymous namespace

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
    // Counters for the number of invocations
    int numAllocaInvocations = 0;
    int numEscapingAllocaInvocations = 0;
    int numDynamicallySizedAllocaInvocations = 0;
    int numDynamicallySizedEscapingAllocaInvocations = 0;

    // Counters to track the amount of memory allocated
    uint64_t totalStaticallyAllocatedStackMemory = 0;
    uint64_t totalStaticallyAllocatedEscapingStackMemory = 0;

    llvm::DataLayout dataLayout(module.get());

    for (const llvm::BasicBlock& basicBlock : function.getBasicBlockList()) {
        for (const llvm::Instruction& instruction : basicBlock.getInstList()) {

            // We're only interested in stack allocations
            if (instruction.getOpcode() != llvm::Instruction::Alloca)
                continue;

            numAllocaInvocations++;

            const auto& allocation = reinterpret_cast<const llvm::AllocaInst&>(instruction);
            bool isStaticAllocation = allocation.isStaticAlloca();
            uint64_t allocationSize;

            if (isStaticAllocation) {
                allocationSize = getAllocationStaticSize(allocation, dataLayout);
                totalStaticallyAllocatedStackMemory += allocationSize;
            }
            else {
                numDynamicallySizedAllocaInvocations++;
            }

            // Run the analysis
            bool returnCaptures = true;
            bool storeCaptures = true;
            if (llvm::PointerMayBeCaptured(&allocation, returnCaptures, storeCaptures, MAX_USES_TO_EXPLORE)) {

                // We found an escaping allocation
                numEscapingAllocaInvocations++;

                // Update counters/size trackers
                if (isStaticAllocation) {
                    totalStaticallyAllocatedEscapingStackMemory += allocationSize;
                }
                else {
                    numDynamicallySizedEscapingAllocaInvocations++;
                }

                if (verbose) {
                    std::cerr << "Capture found\n"
                              << "    function: " << llvm::demangle(function.getName().data()) << "\n"
                              << "        dump: ";
                    instruction.dump();
                }
            }
        }
    }

    std::string functionName = llvm::demangle(function.getName().data());
    writer.addEntry(FunctionSummary{
            .fileName = module->getSourceFileName(),
            .functionName = functionName,
            .numAllocaInvocations = numAllocaInvocations,
            .numEscapingAllocaInvocations = numEscapingAllocaInvocations,
            .numDynamicallySizedAllocaInvocations = numDynamicallySizedAllocaInvocations,
            .numDynamicallySizedEscapingAllocaInvocations = numDynamicallySizedEscapingAllocaInvocations,
            .totalStaticallyAllocatedStackMemory = totalStaticallyAllocatedStackMemory,
            .totalStaticallyAllocatedEscapingStackMemory = totalStaticallyAllocatedEscapingStackMemory
    });
}

} // namespace EscapeAnalysisTool
