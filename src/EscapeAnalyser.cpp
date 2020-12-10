#include "EscapeAnalyser.h"

#include <llvm/Analysis/CaptureTracking.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "CSVWriter.h"

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
//        std::cout << "Found function: " << function.getName().data() << std::endl;

        functionsCount++;
        if (pointerToLocalVariableMayEscape(function))
            unsafeFunctionsCount++;
    }

    writer.addEntry(filePath, functionsCount, unsafeFunctionsCount);
}

bool EscapeAnalyser::pointerToLocalVariableMayEscape(const llvm::Function& function)
{
    // TODO: Implement this!
    return false;
}


} // namespace EscapeAnalysisTool
