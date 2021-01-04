#include <iostream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#include <cxxopts.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>

#include "EscapeAnalyser.h"
#include "CSVWriter.h"

using namespace EscapeAnalysisTool;

namespace logging = boost::log;

int main(int argc, char **argv)
{
    // Parse command line arguments
    cxxopts::Options options(
            "EscapeAnalysisTool",
            "A tool for performing escape analysis on stack-allocated variables");
    std::vector<std::string> inputFiles;
    options.add_options()
            ("i,inputs", "The files to analyse", cxxopts::value<std::vector<std::string>>(inputFiles))
            ("o,output", "The CSV file to write the output to", cxxopts::value<std::string>())
            ("v,verbose", "Log verbose information about the tool's progress", cxxopts::value<bool>()->default_value("false"))
            ("d,debug", "Log additional debugging information", cxxopts::value<bool>()->default_value("false"));
    options.parse_positional({"inputs"});

    std::string outputFile;
    bool verbose, debug;
    std::string errorMessage = "Invalid usage. Expected usage: EscapeAnalysisTool -i input1.ll input2.ll -o myOutput.csv";
    try {
        auto result = options.parse(argc, argv);
        outputFile = result["o"].as<std::string>();
        verbose = result["v"].as<bool>();
        debug = result["d"].as<bool>();
    } catch (cxxopts::OptionParseException& exception) {
        std::cerr << errorMessage << std::endl;
        exit(1);
    } catch (std::domain_error& exception) {
        std::cerr << errorMessage << std::endl;
        exit(1);
    }

    // Set the logging level
    if (debug) {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
    }
    else {
        logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
    }

    // Create a CSVWriter object to the desired output file
    CSVWriter writer(outputFile);

    // Set up the escape analysis tool
    llvm::LLVMContext context;
    llvm::SMDiagnostic err;
    EscapeAnalyser analyser(context, err, writer, verbose, debug);

    // Analyse each file
    for (const std::string& path : inputFiles) {
        analyser.processFile(path);
    }

    return 0;
}
