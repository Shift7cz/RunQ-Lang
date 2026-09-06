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

    // helper parse for each type of statement.;
    std::unique_ptr<Node> parseFn();
    std::unique_ptr<Node> parseRet();
    std::unique_ptr<Node> parseIf();
    std::unique_ptr<Node> parseVarDeclare();
    std::unique_ptr<Node> parseVarLoad();
    std::unique_ptr<Node> parseIntLiteral();
    std::unique_ptr<Node> parseFloatLiteral();
    std::unique_ptr<Node> parseBoolLiteral();
    std::unique_ptr<Node> parseCharLiteral();
    std::unique_ptr<Node> parseMathOperator(TokenType opType, std::unique_ptr<Node> operand1);
    std::unique_ptr<Node> parseExpresion();
};
