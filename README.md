# EscapeAnalysisTool

*WARNING: This tool is work-in-progress, it doesn't do anything meaningful yet!*

This is a tool to perform escape analysis on C/C++ code using LLVM's capture tracking library. I'll be using it to perform a survey of stack memory usage in popular open-source applications for my Master's project.

## Example Usage
```
$ ./EscapeAnalysisTool -i sample.ll -o results.csv
```
`results.csv`:
```
FileName,NumFunctions,NumEscapingFunctions
samples/sample.ll,2,0
```

## Dependencies (vcpkg)
- `llvm`
- `cxxopts`
