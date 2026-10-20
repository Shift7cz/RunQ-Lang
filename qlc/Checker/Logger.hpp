#pragma once
#include <string>

class Log {
public:
    static void error(const std::string& message, size_t line, size_t column, std::string source);
    static void warning(const std::string& message, size_t line, size_t column, std::string source);
    static void log(const std::string& message);
    static bool hasErrors();
private:
    static bool errorOccurred;
};
