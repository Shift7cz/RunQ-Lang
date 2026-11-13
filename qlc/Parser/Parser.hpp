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

    Type getType(); // advances past type if theres any and returns the appropriate type struct
    Type makeType(Token token); // same as getType but you provide the token

public:
    Ast ast;

    Parser(Lexer& lexer) : lexer(lexer) {}

    void parse(); // main parse

    std::vector<std::unique_ptr<AstNode>> parseBlock(bool stopAtBrace);

    // helper parse for each type of statement.;
    std::unique_ptr<AstNode> parseFn();
    std::unique_ptr<AstNode> parseExternFn();
    std::unique_ptr<AstNode> parseEntry();
    std::unique_ptr<AstNode> parseRet();
    std::unique_ptr<AstNode> parseIf();
    std::unique_ptr<AstNode> parseWhile();
    std::unique_ptr<AstNode> parseVarDeclare();
    std::unique_ptr<AstNode> parseLoad();
    std::unique_ptr<AstNode> parseVarMod();
    std::unique_ptr<AstNode> parseIntLiteral();
    std::unique_ptr<AstNode> parseFloatLiteral();
    std::unique_ptr<AstNode> parseBoolLiteral();
    std::unique_ptr<AstNode> parseCharLiteral();

    std::unique_ptr<AstNode> parsePrimary();

    std::unique_ptr<AstNode> parseMultiplicative();

    std::unique_ptr<AstNode> parseComparison();
    std::unique_ptr<AstNode> parseExpresion();
};
