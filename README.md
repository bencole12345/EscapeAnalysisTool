# EscapeAnalysisTool

This is a tool to perform escape analysis on C/C++ code using LLVM's capture tracking library. I'll be using it to perform a survey of stack memory usage in popular open-source applications for my Master's project.

At the moment, you have to compile the source files to LLVM IR yourself.

## Example Usage
```
$ clang++ -O0 -S -emit-llvm samples/sample1.cpp -o samples/sample1.ll
$ clang++ -O0 -S -emit-llvm samples/sample2.cpp -o samples/sample2.ll
$ ./EscapeAnalysisTool -i samples/sample*.ll -o results.csv
```
`results.csv`:
```
FileName;FunctionName;NumStackAllocations;NumEscapingStackAllocations;TotalAllocatedStackMemory;TotalEscapingAllocateStackMemory
samples/sample.cpp;main;1;0;4;0
samples/sample.cpp;anotherFunction(int, int);2;0;8;0
samples/sample.cpp;escapingFunction();1;1;4;4
samples/sample.cpp;nonEscapingFunction(int*);2;0;12;0
samples/sample.cpp;definitelyNonEscapingFunction(int*);1;0;8;0
samples/sample2.cpp;allocateAnArrayOfTwelveInts();1;0;48;0
```

Note: This tool emits CSV files in semicolon-separated form because the C++ function names may include commas.

## Arguments
- `-i <input files>` [Required]

    The input files to the tool. You must pass at least one `.ll` file to the program.

- `-o <output file>` [Required]

    The output file. This is where the results be written in semicolon-separated CSV format.

- `-v` [Optional]

    Verbose flag. This will print verbose output while the tool is working, mostly useful just for debugging purposes.

## Dependencies (vcpkg)
- `llvm`
- `cxxopts`
