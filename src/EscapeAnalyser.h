#ifndef ESCAPEANALYSISTOOL_ESCAPEANALYSER
#define ESCAPEANALYSISTOOL_ESCAPEANALYSER

#include <string>

#include <llvm/Analysis/CaptureTracking.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/SourceMgr.h>

#include "CSVWriter.h"

namespace EscapeAnalysisTool {

/**
 * Drives LLVM's escape analysis on `.ll` files, writing the results to a
 * supplied `CSVWriter`.
 */
class EscapeAnalyser {
private:
    /**
     * Both required for parsing the LLVM IR code.
     */
    llvm::LLVMContext& context;
    llvm::SMDiagnostic& err;

    /**
     * Where to write the CSV output to.
     */
    CSVWriter& writer;

    /**
     * Whether to log additional information to std::cout.
     */
    bool verbose;

    /**
     * Whether to dump any capture that we find to std::cout.
     */
    bool dumpAllCaptures;

public:
    EscapeAnalyser(llvm::LLVMContext& context, llvm::SMDiagnostic& err, CSVWriter& writer,
                   bool verbose = false, bool dumpAllCaptures = false);

    /**
     * Performs the analysis on a .ll file, writing the results to `writer`.
     *
     * @param filePath The file to process
     */
    void processFile(const std::string& filePath);

private:
    /**
     * Performs the analysis on a single function, writing the results to `writer`.
     * @param function
     */
    void processFunction(const llvm::Function& function, std::unique_ptr<llvm::Module>& module);

};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_ESCAPEANALYSER
