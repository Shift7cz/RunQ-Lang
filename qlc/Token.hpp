#pragma once
#include <string_view>
#include <iostream>
#include <iomanip>

enum class TokenType {
    Identifier, // eg "main"

    // keyword
    Return,
    Fn,

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

inline std::string_view tokenTypeToString(TokenType type) { // helper for debuging, temporery
    switch (type) {
        case TokenType::Identifier:  return "Identifier";
        case TokenType::Return:      return "Return";
        case TokenType::Fn:          return "Fn";
        case TokenType::I32Literal:  return "I32Literal";
        case TokenType::OpenParen:   return "OpenParen";
        case TokenType::ClosedParen: return "ClosedParen";
        case TokenType::OpenBrace:   return "OpenBrace";
        case TokenType::ClosedBrace: return "ClosedBrace";
        case TokenType::Semicolon:   return "Semicolon";
        case TokenType::EndOfFile:   return "EndOfFile";
        case TokenType::Unknown:     return "Unknown";
    }
    return "InvalidType"; // Fallback safety
}

// Beautifully format the Token output
inline std::ostream& operator<<(std::ostream& os, const Token& token) { // helper for debuging, temporery
    os << "[" << token.line << ":" << token.column << "] "
       << std::left << std::setw(15) << tokenTypeToString(token.type)
       << " -> '" << token.data << "'";
    return os;
}