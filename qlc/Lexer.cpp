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
        case ':': advance(); return Token{TokenType::Colon, ":", line, column};
            //case '=': advance(); return Token{TokenType::Equals, "=", line, column};

            // Math Operators
        case '+': advance(); return Token{TokenType::Plus, "+", line, column};
        case '-': advance(); return Token{TokenType::Dash, "-", line, column};
        case '*': advance(); return Token{TokenType::Star, "*", line, column};
        case '/': advance(); return Token{TokenType::Slash, "/", line, column};

            // starting with the same character handling here
        case '=':
            advance();
            if (peak() == '=') {
                advance(); return Token{TokenType::IsEqualTo, "==", line, column};
            }
            advance(); return Token{TokenType::Equals, "=", line, column};
        case '<':
            advance();
            if (peak() == '=') {
                advance(); return Token{TokenType::LessOrEqual, "<=", line, column};
            }
            advance(); return Token{TokenType::Equals, "<", line, column};
        case '>':
            advance();
            if (peak() == '=') {
                advance(); return Token{TokenType::LessOrEqual, ">=", line, column};
            }
            advance(); return Token{TokenType::Equals, ">", line, column};
        case '!': advance(); advance(); return Token{TokenType::NotEqualTo, "!=", line, column}; // todo: handling of the bitwise !boolValue operation same as with = < >
    }

    if (isdigit(peak()) || peak() == '.') {
        size_t startPos = pos;

        while (isdigit(peak()) || peak() == '.') {
            advance();
        }

        std::string_view nextToken = source.substr(startPos, pos - startPos);

        if (nextToken.contains('.')) {
            return Token{TokenType::FloatLiteral, nextToken, line, column};
        }
        return Token{TokenType::IntLiteral, nextToken, line, column};
    }

    if (isalnum(peak()) || peak() == '_' || peak() == '"' || peak() == '\'') {
        size_t startPos = pos;

        while (isalnum(peak()) || peak() == '_' || peak() == '"' || peak() == '\'') {
            advance();
        }

        std::string_view nextToken = source.substr(startPos, pos - startPos);

        // todo: sort them based on use to be faster?
        if (nextToken == "return") return Token{TokenType::Return, nextToken, line, column};
        if (nextToken == "fn") return Token{TokenType::Fn, "fn", line, column};
        if (nextToken == "let") return Token{TokenType::Let, "let", line, column};
        if (nextToken == "i32") return Token{TokenType::I32, "i32", line, column};
        if (nextToken == "i8") return Token{TokenType::I8, "i8", line, column};
        if (nextToken == "f64") return Token{TokenType::F64, "f64", line, column};
        if (nextToken == "bool") return Token{TokenType::Bool, "bool", line, column};
        if (nextToken == "char") return Token{TokenType::Char, "char", line, column};
        if (nextToken == "if") return Token{TokenType::If, "if", line, column};
        if (nextToken == "else") return Token{TokenType::Else, "else", line, column};

        if (nextToken == "true" || nextToken == "false") {
            return Token{TokenType::BoolLiteral, nextToken, line, column};
        }
        if (nextToken[0] == '\'' && nextToken.back() == '\'') {
            std::string_view data;
            switch (nextToken.length()) {
                case 3:
                    data = nextToken.substr(1, 1);
                    break;
                case 4: // todo: test this actually works as intended (used for \n, \', etc.)
                    data = nextToken.substr(1, 2);
            }
            return Token{TokenType::CharLiteral, data, line, column};
        }

        return Token{TokenType::Identifier, nextToken, line, column};
    }

    if (peak() == '\0') {
        return Token{TokenType::EndOfFile, "\0", line, column};
    }

    advance();
    return Token{TokenType::Unknown, "", line, column}; // todo: proper bad token return
}