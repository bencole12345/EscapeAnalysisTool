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

struct EscapeAnalyserSettings {

    /**
     * Whether to log additional progress information.
     */
    bool verbose {false};

    /**
     * Whether to log every single function processed.
     */
    bool superVerbose {false};

    /**
     * Whether to keep functions containing zero instructions.
     * 
     * These are typically forward declarations, which we may want to exclude
     * from the analysis.
     */
    bool keepEmptyFunctions {false};

    /**
     * Whether instructions containing escaping allocations should be logged.
     * 
     * This is for debugging purposes only and is not expected to be used in
     * normal operation.
     */
    bool printDebugInfo {false};

};

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
     * The settings that the user supplied when the analyser was created.
     */
    EscapeAnalyserSettings settings;

public:
    EscapeAnalyser(llvm::LLVMContext& context, llvm::SMDiagnostic& err, CSVWriter& writer,
                   EscapeAnalyserSettings settings);

    /**
     * Performs the analysis on a .ll file, writing the results to `writer`.
     *
     * @param filePath The file to process
     */
    void processFile(const std::string& filePath);

private:
    /**
     * Performs the analysis on a single function, writing the results to `writer`.
     * 
     * @param function The function to process
     */
    void processFunction(const llvm::Function& function, std::unique_ptr<llvm::Module>& module);

};

} // namespace EscapeAnalysisTool

#endif //ESCAPEANALYSISTOOL_ESCAPEANALYSER
