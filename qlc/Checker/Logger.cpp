#include "Logger.hpp"

#include <iostream>

void Log::error(const std::string &message, size_t line, size_t column, std::string source) {
    std::cerr << "ERROR: [source " << source << " | line " << line << "| column: " << column << "] -> " << message << "\n";
    errorOccurred = true;
}

void Log::warning(const std::string &message, size_t line, size_t column, std::string source) {
    std::cout << "Warning: [source " << source << " | line " << line << " | column: " << column << "] -> " << message << "\n";
}

void Log::log(const std::string &message) {
    std::cout << message << std::endl;
}

bool Log::hasErrors() {
    return errorOccurred;
}

bool Log::errorOccurred = false;