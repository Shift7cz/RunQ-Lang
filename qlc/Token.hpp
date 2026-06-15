#pragma once
#include <string_view>

enum class TokenType {
    // keyword
    Identifier, // eg "main"
    Return,

    // literal (raw data)
    I32Literal,

    // Symbols
    OpenParen, // '('
    ClosedParen, // ')'
    OpenBrace, // '{'
    ClosedBrace, // '}'
    Semicolon, // ';'

    // Special
    EndOfFile,
    Unknown // for unknown
};

struct Token {
    TokenType type;
    std::string_view data;
    size_t line;
    size_t column;
};