#pragma once

#include "Lexer.hpp"
#include "Ast.hpp"

class Parser {
private:
    Lexer& lexer;
    Token currentToken;

    void advance();
    bool expect(TokenType expected);
    bool expectAndConsume(TokenType expected);

public:
    Ast ast;

    Parser(Lexer& lexer) : lexer(lexer) {}

    void parse(); // main parse

    // helper parse for each type of statement.
    std::unique_ptr<Node> parseFn();
    std::unique_ptr<Node> parseRet();
};
