#include "ArgumentParser.h"

#include <iostream>
#include <string>
#include <vector>

namespace EscapeAnalysisTool {

void parseArguments(std::vector<std::string>* arguments, int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Invalid usage: please include at least one file to analyse" << std::endl;
        exit(1);
    }

    arguments->clear();

    for (size_t i = 1; i < argc; i++) {
        arguments->push_back(argv[i]);
    }
}

} // namespace EscapeAnalysisTool
