#include "Lexer.hpp"

#include <string>
#include <cctype>

Lexer::Lexer(std::string_view src) {
    line = 0;
    column = 0;
    pos = 0;
    source = src;
}

char Lexer::advance() {
    if (pos >= source.length()) {
        return '\0';
    }
    char current = source[pos];
    pos++;

    if (current == '\n') {
        line++;
        column = 0;
    }
    else {
        column++;
    }

    return current;
}

char Lexer::peak() {
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

bool Lexer::skipWhiteSpace() {
    if (peak() == ' ' || peak() == '\t' || peak() == '\n') {
        advance();
        return true;
    }
    return false;
}


Token Lexer::nextToken() {
    while (skipWhiteSpace()) {}

    switch (peak()) {
            // Symbols
        case '(': advance(); return Token{TokenType::OpenParen, "(", line, column};
        case ')': advance(); return Token{TokenType::ClosedParen, ")", line, column};
        case '{': advance(); return Token{TokenType::OpenBrace, "{", line, column};
        case '}': advance(); return Token{TokenType::ClosedBrace, "}", line, column};
        case ';': advance(); return Token{TokenType::Semicolon, ";", line, column};
    }

    if (isdigit(peak()) || peak() == '.') {
        size_t startPos = pos;

        while (isdigit(peak()) || peak() == '.') {
            advance();
        }

        std::string_view nextToken = source.substr(startPos, pos - startPos);

        // if (nextToken.contains('.')) { todo: to be implemented
        //     return Token{DECIMAL_NUMBER, nextToken, line, column};
        // }
        return Token{TokenType::I32Literal, nextToken, line, column};

    }

    if (isalnum(peak()) || peak() == '_' || peak() == '"' || peak() == '\'') {
        size_t startPos = pos;

        while (isalnum(peak()) || peak() == '_' || peak() == '"' || peak() == '\'') {
            advance();
        }

        std::string_view nextToken = source.substr(startPos, pos - startPos);

        if (nextToken == "return") {
            return Token{TokenType::Return, nextToken, line, column};
        }
        if (nextToken == "fn") {
            return Token{TokenType::Fn, "fn", line, column};
        }

        return Token{TokenType::Identifier, nextToken, line, column};
    }

    if (peak() == '\0') {
        return Token{TokenType::EndOfFile, "\0", line, column};
    }

    advance();
    return Token{TokenType::Unknown, "", line, column}; // todo: proper bad token return
}
