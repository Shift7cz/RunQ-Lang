#include "Lexer.hpp"

#include <string>

Lexer::Lexer() {
    line = 0;
    column = 0;
    pos = 0;
}

char Lexer::advance(std::string source) {
    // if is_at_end {return a unique end identifier}
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

Token Lexer::nextToken(std::string source) {
    std::string nextToken;
    nextToken += advance(source);

    switch (nextToken[0]) {
        case '(': return Token{TokenType::OpenParen, nextToken, line, column};
        case ')': return Token{TokenType::ClosedParen, nextToken, line, column};
        case '{': return Token{TokenType::OpenBrace, nextToken, line, column};
        case '}': return Token{TokenType::ClosedBrace, nextToken, line, column};
        case ';': return Token{TokenType::Semicolon, nextToken, line, column};
    }

    // advance until bigger token match in a loop, check if token isn't value.
}
