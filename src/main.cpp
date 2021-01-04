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
            "A tool for performing escape analysis on stack-allocated variables.");
    std::vector<std::string> inputFiles;
    options.add_options("Inputs and Outputs")
            ("i,inputs", "The input files to analyse", cxxopts::value<std::vector<std::string>>(inputFiles))
            ("o,output", "The CSV file to write the output to", cxxopts::value<std::string>()->default_value("output.csv"), "output_file.csv");
    options.add_options("Parameters")
            ("k,keep", "Keep empty functions rather than discarding them", cxxopts::value<bool>()->default_value("false"));
    options.add_options("Logging")
            ("v,verbose", "Log verbose information about the tool's progress", cxxopts::value<bool>()->default_value("false"))
            ("V,superVerbose", "Log every single function processed", cxxopts::value<bool>()->default_value("false"))
            ("d,debug", "Log additional debugging information", cxxopts::value<bool>()->default_value("false"));
    options.add_options("Help")
            ("h,help", "Display usage help", cxxopts::value<bool>()->default_value("false"));
    options.parse_positional({"inputs"});
    options.positional_help("input_file_1.(ll|bc) input_file_2.(ll|bc) ...");

    EscapeAnalyserSettings settings;
    std::string outputFile;

    std::string errorMessage = "Invalid arguments: run `EscapeAnalysisTool -h` for help.";
    bool displayHelp;
    try {
        auto result = options.parse(argc, argv);
        displayHelp = result["h"].as<bool>();

        if (!displayHelp) {
            outputFile = result["o"].as<std::string>();
            settings.verbose = result["v"].as<bool>() || result["V"].as<bool>();
            settings.superVerbose = result["V"].as<bool>();
            settings.printDebugInfo = result["d"].as<bool>();
            settings.keepEmptyFunctions = result["k"].as<bool>();
        }
        
    } catch (cxxopts::OptionParseException& exception) {
        std::cerr << errorMessage << std::endl;
        exit(1);
    } catch (std::domain_error& exception) {
        std::cerr << errorMessage << std::endl;
        exit(1);
    }

    if (displayHelp) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    // Set the logging level
    if (settings.printDebugInfo) {
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
    EscapeAnalyser analyser(context, err, writer, settings);

    // Analyse each file
    for (const std::string& path : inputFiles) {
        analyser.processFile(path);
    }

    return 0;
}
