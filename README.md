# EscapeAnalysisTool

*WARNING: This tool is work-in-progress, it doesn't do anything meaningful yet!*

This is a tool to perform escape analysis on C/C++ code using LLVM's capture tracking library. I'll be using it to perform a survey of stack memory usage in popular open-source applications for my Master's project.

## Example Usage
```
$ ./EscapeAnalysisTool sample.ll > results.csv
```
`results.csv`:
```
FileName,NumFunctions,NumEscapingFunctions
samples/sample.ll,2,0
```

## Dependencies
Currently the only dependency is llvm. The command `vcpkg install llvm` is enough for me to get it to build.