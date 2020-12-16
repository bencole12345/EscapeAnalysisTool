cmake_minimum_required(VERSION 3.12)
project(EscapeAnalysisTool)

# Required for LLVM
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

# Need these to get LLVM to work
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_executable(EscapeAnalysisTool
        src/main.cpp
        src/CSVWriter.cpp
        src/EscapeAnalyser.cpp
        src/FunctionSummary.cpp)

# Determine which libraries we need to link against for LLVM
llvm_map_components_to_libnames(llvm_libs support core irreader analysis)

target_link_libraries(EscapeAnalysisTool
        PRIVATE
            ${llvm_libs}
            cxxopts::cxxopts)
