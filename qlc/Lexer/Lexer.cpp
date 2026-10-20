#include "Lexer.hpp"

#include <string>
#include <cctype>

Lexer::Lexer(std::string cde, std::string src) {
    line = 0;
    column = 0;
    pos = 0;
    code = cde;
    source = src;
}

char Lexer::advance() {
    if (pos >= code.length()) {
        return '\0';
    }
    char current = code[pos];
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
    if (pos >= code.length()) {
        return '\0';
    }
    return code[pos];
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
        case '(': advance(); return Token{TokenType::OpenParen, "(", line, column, source};
        case ')': advance(); return Token{TokenType::ClosedParen, ")", line, column, source};
        case '{': advance(); return Token{TokenType::OpenBrace, "{", line, column, source};
        case '}': advance(); return Token{TokenType::ClosedBrace, "}", line, column, source};
        case ';': advance(); return Token{TokenType::Semicolon, ";", line, column, source};
        case ':': advance(); return Token{TokenType::Colon, ":", line, column, source};
        case '~': advance(); return Token{TokenType::Tilde, "~", line, column, source};

            // Math Operators
        case '+': advance(); return Token{TokenType::Plus, "+", line, column, source};
        case '-': advance(); return Token{TokenType::Dash, "-", line, column, source};
        case '*': advance(); return Token{TokenType::Star, "*", line, column, source};
        //case '/': advance(); return Token{TokenType::Slash, "/", line, column};
        case ',': advance(); return Token{TokenType::Comma, ",", line, column, source};

            // starting with the same character handling here
        case '=':
            advance();
            if (peak() == '=') {
                advance(); return Token{TokenType::IsEqualTo, "==", line, column, source};
            }
            advance(); return Token{TokenType::Equals, "=", line, column, source};
        case '<':
            advance();
            if (peak() == '=') {
                advance(); return Token{TokenType::LessOrEqual, "<=", line, column, source};
            }
            advance(); return Token{TokenType::LessThan, "<", line, column, source};
        case '>':
            advance();
            if (peak() == '=') {
                advance(); return Token{TokenType::GreaterOrEqual, ">=", line, column, source};
            }
            advance(); return Token{TokenType::GreaterThan, ">", line, column, source};

        case '/':
            advance();
            if (peak() == '/') {
               // its a comment
                while (peak() != '\n' && peak() != '\0') {
                    advance();
                }
                return Token{TokenType::Comment, "", line, column, source};
            }\
            advance(); return Token{TokenType::Slash, "/", line, column, source};

        case '!': advance(); advance(); return Token{TokenType::NotEqualTo, "!=", line, column, source}; // todo: handling of the bitwise !boolValue operation same as with = < >
    }

    if (isdigit(peak()) || peak() == '.') {
        size_t startPos = pos;

        while (isdigit(peak()) || peak() == '.') {
            advance();
        }

        std::string_view nextToken = code.substr(startPos, pos - startPos);

        if (nextToken.contains('.')) {
            return Token{TokenType::FloatLiteral, std::string(nextToken), line, column, source};
        }
        return Token{TokenType::IntLiteral, std::string(nextToken), line, column, source};
    }

    if (isalnum(peak()) || peak() == '_' || peak() == '"' || peak() == '\'') {
        size_t startPos = pos;

        while (isalnum(peak()) || peak() == '_' || peak() == '"' || peak() == '\'') {
            advance();
        }

        std::string_view nextToken = code.substr(startPos, pos - startPos);

        // todo: sort them based on use to be faster? and make it a switch?
        if (nextToken == "return") return Token{TokenType::Return, "return", line, column, source};
        if (nextToken == "fn") return Token{TokenType::Fn, "fn", line, column, source};
        if (nextToken == "let") return Token{TokenType::Let, "let", line, column, source};
        if (nextToken == "i32") return Token{TokenType::I32, "i32", line, column, source};
        if (nextToken == "i8") return Token{TokenType::I8, "i8", line, column, source};
        if (nextToken == "f64") return Token{TokenType::F64, "f64", line, column, source};
        if (nextToken == "bool") return Token{TokenType::Bool, "bool", line, column, source};
        if (nextToken == "char") return Token{TokenType::Char, "char", line, column, source};
        if (nextToken == "if") return Token{TokenType::If, "if", line, column, source};
        if (nextToken == "else") return Token{TokenType::Else, "else", line, column, source};
        if (nextToken == "while") return Token{TokenType::While, "while", line, column, source};
        if (nextToken == "entry") return Token{TokenType::Entry, "entry", line, column, source};
        if (nextToken == "extern") return Token{TokenType::Extern, "extern", line, column, source};

        if (nextToken == "true" || nextToken == "false") {
            return Token{TokenType::BoolLiteral, std::string(nextToken), line, column, source};
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
            return Token{TokenType::CharLiteral, std::string(data), line, column, source};
        }

        return Token{TokenType::Identifier, std::string(nextToken), line, column, source};
    }

    if (peak() == '\0') {
        return Token{TokenType::EndOfFile, "\0", line, column, source};
    }

    advance();
    return Token{TokenType::Unknown, "", line, column, source}; // todo: proper bad token return
}