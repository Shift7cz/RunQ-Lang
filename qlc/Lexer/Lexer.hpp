#pragma once
#include "Token.hpp"

class Lexer {
public:
    size_t line; // on what line is it in file (tracks \n)
    size_t column; // tracks collum (resets with \n)
    size_t pos; // general string of file position (doesn't reset)
    std::string code;
    std::string source;

    Lexer(std::string cde, std::string src);

    char advance();
    char peak();
    bool skipWhiteSpace();

    Token nextToken();
};