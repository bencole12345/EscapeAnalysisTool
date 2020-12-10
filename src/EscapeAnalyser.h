#ifndef ESCAPEANALYSISTOOL_ESCAPEANALYSER
#define ESCAPEANALYSISTOOL_ESCAPEANALYSER

#include <string>

#include <llvm/Analysis/CaptureTracking.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>

#include "CSVWriter.h"

namespace EscapeAnalysisTool {

class EscapeAnalyser {
private:
    llvm::LLVMContext& context;
    llvm::SMDiagnostic& err;

public:
    EscapeAnalyser(llvm::LLVMContext& context, llvm::SMDiagnostic& err);

    void processFile(const std::string& filePath, CSVWriter& writer);
    bool pointerToLocalVariableMayEscape(const llvm::Function& function);
};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_ESCAPEANALYSER
