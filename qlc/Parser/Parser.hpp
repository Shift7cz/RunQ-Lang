#pragma once

#include "../Lexer/Lexer.hpp"
#include "Ast.hpp"

class Parser {
private:
    Lexer& lexer;
    Token currentToken;

    void advance();
    bool expect(TokenType expected);
    bool expectAndConsume(TokenType expected, bool logError);

public:
    Ast ast;

    Parser(Lexer& lexer) : lexer(lexer) {}

    void parse(); // main parse

    std::vector<std::unique_ptr<Node>> parseBlock(bool stopAtBrace);

    // helper parse for each type of statement.;
    std::unique_ptr<Node> parseFn();
    std::unique_ptr<Node> parseExternFn();
    std::unique_ptr<Node> parseEntry();
    std::unique_ptr<Node> parseRet();
    std::unique_ptr<Node> parseIf();
    std::unique_ptr<Node> parseWhile();
    std::unique_ptr<Node> parseVarDeclare();
    std::unique_ptr<Node> parseLoad();
    std::unique_ptr<Node> parseVarMod();
    std::unique_ptr<Node> parseIntLiteral();
    std::unique_ptr<Node> parseFloatLiteral();
    std::unique_ptr<Node> parseBoolLiteral();
    std::unique_ptr<Node> parseCharLiteral();

    std::unique_ptr<Node> parsePrimary();

    std::unique_ptr<Node> parseMultiplicative();

    std::unique_ptr<Node> parseComparison();
    std::unique_ptr<Node> parseExpresion();
};
