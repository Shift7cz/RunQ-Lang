#pragma once
#include <string>
#include <vector>
#include <filesystem>

enum CompilerFlags {
    DontCompile,
    DevMode,
    IsLibrary,
    EmitAsm,
};

class Project {
public:
    std::vector<std::string> runqList;
    std::vector<std::string> cList;
    std::vector<std::string> libList;

    std::vector<CompilerFlags> flagList;

    void load(std::string path);
};