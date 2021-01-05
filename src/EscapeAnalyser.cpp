#include "EscapeAnalyser.h"

#include <algorithm>
#include <string>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

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

EscapeAnalyser::EscapeAnalyser(llvm::LLVMContext& context, llvm::SMDiagnostic& err, CSVWriter& writer,
                               EscapeAnalyserSettings settings)
    : context(context), err(err), writer(writer), settings(settings)
{
}

void EscapeAnalyser::processFile(const std::string& filePath)
{
    BOOST_LOG_TRIVIAL(info) << "Processing file: " << filePath;

    if (settings.verbose) {
        BOOST_LOG_TRIVIAL(info) << "Parsing...";
    }

    // TODO: Use the lazily loaded module option here instead
    std::unique_ptr<llvm::Module> module(llvm::parseIRFile(filePath, err, context));
    if (!module) {
        BOOST_LOG_TRIVIAL(fatal) << "Bad input file: " << filePath;
        exit(2);
    }

    int numFunctions = module->getFunctionList().size();
    if (settings.verbose) {
        BOOST_LOG_TRIVIAL(info) << "Found " << numFunctions << " functions, processing now...";
    }

    int count = 1;
    for (const llvm::Function& function : module->functions()) {

        bool requiresProcessing = function.getInstructionCount() > 0 || settings.keepEmptyFunctions;

        if (settings.verbose && count % 1000 == 0) {
            float percentageComplete = (float)count / (float)numFunctions;
            BOOST_LOG_TRIVIAL(info) << "Progress: " << percentageComplete << "%";
        }

        else if (settings.superVerbose) {
            if (requiresProcessing) {
                BOOST_LOG_TRIVIAL(info) << "Processing function " << count << "/" << numFunctions
                                        << ": " << llvm::demangle(function.getName().data());
            }
            else {
                BOOST_LOG_TRIVIAL(info) << "Skipping empty function " << count << "/" << numFunctions
                                        << ": " << llvm::demangle(function.getName().data());
            }
        }

        if (requiresProcessing) {
            processFunction(function, module);
        }

        count++;
    }

    BOOST_LOG_TRIVIAL(info) << "Finished processing file " << filePath;
}

void EscapeAnalyser::processFunction(const llvm::Function& function, std::unique_ptr<llvm::Module>& module)
{
    // Counters for the number of invocations
    unsigned int numAllocaInvocations = 0;
    unsigned int numEscapingAllocaInvocations = 0;
    unsigned int numDynamicallySizedAllocaInvocations = 0;
    unsigned int numDynamicallySizedEscapingAllocaInvocations = 0;

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

                // We found an escaping stack allocation
                numEscapingAllocaInvocations++;

                // Update counters/size trackers
                if (isStaticAllocation) {
                    totalStaticallyAllocatedEscapingStackMemory += allocationSize;
                }
                else {
                    numDynamicallySizedEscapingAllocaInvocations++;
                }

                // Bit of a hack, but both debug logs and dumpAllCaptures are enabled iff the
                // program is invoked with the -d flag. We'll never have one of these enabled
                // without the other.
                if (settings.printDebugInfo) {
                    BOOST_LOG_TRIVIAL(debug) << "Capture found\n"
                            << "    function: " << llvm::demangle(function.getName().data()) << "\n"
                            << "        dump: ";
                    instruction.dump();
                }
            }
        }
    }

    std::string functionName = llvm::demangle(function.getName().data());
    unsigned int numInstructions = function.getInstructionCount();

    writer.addEntry(FunctionSummary{
            .functionName = functionName,
            .numInstructions = numInstructions,
            .numAllocaInvocations = numAllocaInvocations,
            .numEscapingAllocaInvocations = numEscapingAllocaInvocations,
            .numDynamicallySizedAllocaInvocations = numDynamicallySizedAllocaInvocations,
            .numDynamicallySizedEscapingAllocaInvocations = numDynamicallySizedEscapingAllocaInvocations,
            .totalStaticallyAllocatedStackMemory = totalStaticallyAllocatedStackMemory,
            .totalStaticallyAllocatedEscapingStackMemory = totalStaticallyAllocatedEscapingStackMemory
    });
}

} // namespace EscapeAnalysisTool
