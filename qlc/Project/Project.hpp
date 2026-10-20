#pragma once
#include <string>
#include <vector>
#include <filesystem>

class Project {
public:
    std::vector<std::string> runqList;
    std::vector<std::string> cList;
    std::vector<std::string> libList;

    void load(std::string path);
};