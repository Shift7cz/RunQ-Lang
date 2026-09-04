#pragma once
#include <string_view>
#include <iomanip>

enum class TokenType {
    Identifier, // eg "main"

    // keyword
    Return,
    Fn,
    Let,

    // literal (raw data)
    I32Literal,

    // Types
    I32, // i32

    // math operators
    Plus, // '+'
    Dash, // '-'
    Star, // '*'
    Slash, // '/'

    // Symbols
    OpenParen, // '('
    ClosedParen, // ')'
    OpenBrace, // '{'
    ClosedBrace, // '}'
    Semicolon, // ';'
    Colon, // ':'
    Equals, // '='

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

inline std::string_view tokenTypeToString(TokenType type) { // helper, {ai/2}
    switch (type) {
        case TokenType::Identifier:  return "Identifier";
        case TokenType::Return:      return "Return";
        case TokenType::Fn:          return "Fn";
        case TokenType::Let:         return "Let";
        case TokenType::I32Literal:  return "I32Literal";
        case TokenType::I32:         return "I32";
        case TokenType::Plus:        return "Plus";
        case TokenType::Dash:        return "Dash";
        case TokenType::Star:        return "Star";
        case TokenType::Slash:       return "Slash";
        case TokenType::OpenParen:   return "OpenParen";
        case TokenType::ClosedParen: return "ClosedParen";
        case TokenType::OpenBrace:   return "OpenBrace";
        case TokenType::ClosedBrace: return "ClosedBrace";
        case TokenType::Semicolon:   return "Semicolon";
        case TokenType::Colon:       return "Colon";
        case TokenType::Equals:      return "Equals";
        case TokenType::EndOfFile:   return "EndOfFile";
        case TokenType::Unknown:     return "Unknown";
    }
    return "InvalidType"; // Fallback safety
}

// Format the token output
inline std::ostream& operator<<(std::ostream& os, const Token& token) { // helper for debuging, temporery
    os << "[" << token.line << ":" << token.column << "] "
       << std::left << std::setw(15) << tokenTypeToString(token.type)
       << " -> '" << token.data << "'";
    return os;
}