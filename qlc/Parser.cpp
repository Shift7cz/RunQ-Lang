#include "Parser.hpp"

#include <iostream>

void Parser::advance() {
    currentToken = lexer.nextToken();
}

bool Parser::expect(TokenType expected) {
    if (currentToken.type == expected) {
        return true;
    }
    return false;
}

bool Parser::expectAndConsume(TokenType expected) {
    if (!expect(expected)) {
        // todo: add syntax errors.
        return false;
    }
    advance();
    return true;
}

void Parser::parse() {
    advance();

    while (currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::Fn) {
            std::unique_ptr<Node> funcNode = parseFn();

            if (funcNode != nullptr) {
                ast.root = std::move(funcNode);
            }
            else {
                std::cout << "Parser error" << std::endl;
            }
        }
        else {
            advance();
        }
    }
}

std::unique_ptr<Node> Parser::parseFn() {
    expectAndConsume(TokenType::Fn); // move past fn

    if (!expect(TokenType::Identifier)) return nullptr;
    std::string_view identifier = currentToken.data;
    advance(); // move past identifier

    if (!expectAndConsume(TokenType::OpenParen)) return nullptr;

    // todo: implement parsing parameters

    if (!expectAndConsume(TokenType::ClosedParen)) return nullptr;

    TokenType retType = TokenType::Unknown;
    if (expectAndConsume(TokenType::Colon)) {
        // moves into return type if return type is available
        if (expect(TokenType::I32)) { // todo: dont forget other return types
            retType = currentToken.type;
            advance();
        }
        else {
            // todo: error handling
            return nullptr;
        }
    }

    if (!expectAndConsume(TokenType::OpenBrace)) return nullptr;

    auto funcNode = std::make_unique<FuncNode>();
    funcNode->identifier = identifier;
    funcNode->returnType = retType;

    while (!expect(TokenType::ClosedBrace) && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::Return) {
            std::unique_ptr<Node> retNode = parseRet();
            funcNode->body.push_back(std::move(retNode));
        }
        else if (currentToken.type == TokenType::Let) {
            std::unique_ptr<Node> varNode = parseVarDeclare();
            funcNode->body.push_back(std::move(varNode));
        }
        // Todo: other statements
        else {
        advance();
        }
    }

    if (!expectAndConsume(TokenType::ClosedBrace)) return nullptr;

    return funcNode;
}

std::unique_ptr<Node> Parser::parseRet() {
    if (!expectAndConsume(TokenType::Return)) return nullptr;

    if (!expect(TokenType::I32Literal)) return nullptr;

    auto retNode = std::make_unique<ReturnNode>();
    retNode->body = parseExpresion(); // this now leaves cursor right after the literal

    if (!retNode->body) return nullptr;
    if (!expectAndConsume(TokenType::Semicolon)) return nullptr;

    return retNode;
}

std::unique_ptr<Node> Parser::parseVarDeclare() {
    if (!expectAndConsume(TokenType::Let)) return nullptr;

    auto varNode = std::make_unique<VarDeclareNode>();

    if (expect(TokenType::I32)) {
        varNode->valueType = currentToken.type;
    }
    // todo: other else if statements for other data types

    advance();

    if (expect(TokenType::Identifier)) {
        varNode->identifier = currentToken.data;
        advance();
    }
    else {
        return nullptr;
    }

    if (!expectAndConsume(TokenType::Equals)) return nullptr;

    varNode->body = parseExpresion();

    if (!varNode->body) return nullptr;
    if (!expectAndConsume(TokenType::Semicolon)) return nullptr;

    return varNode;
}

std::unique_ptr<Node> Parser::parseI32Literal() {
    if (expect(TokenType::I32Literal)) {
        auto i32Node = std::make_unique<I32LiteralNode>();
        i32Node->value = std::atoi(currentToken.data.data());

        advance();

        return i32Node;
    }
    else {
        return nullptr;
    }
}

std::unique_ptr<Node> Parser::parseMathOperator(TokenType opType, std::unique_ptr<Node> operand1) {
    auto mathOpNode = std::make_unique<MathOperatorNode>();
    mathOpNode->operatorType = opType;

    advance();

    if (expect(TokenType::I32Literal)) {
        std::unique_ptr<Node> operand2 = parseI32Literal(); // todo: Parse expression instead of i32Literal for longer math operations?

        mathOpNode->operand1 = std::move(operand1);
        mathOpNode->operand2 = std::move(operand2);

        return mathOpNode;
    }

    // todo: error handling
    return nullptr;
}


std::unique_ptr<Node> Parser::parseExpresion() {
    if (expect(TokenType::I32Literal)) {
        std::unique_ptr<Node> i32Node1 = parseI32Literal();

        if (expect(TokenType::Plus)) {
            return parseMathOperator(TokenType::Plus, std::move(i32Node1));
        }
        if (expect(TokenType::Dash)) {
            return parseMathOperator(TokenType::Dash, std::move(i32Node1));
        }
        if (expect(TokenType::Star)) {
            return parseMathOperator(TokenType::Star, std::move(i32Node1));
        }
        if (expect(TokenType::Slash)) {
            return parseMathOperator(TokenType::Slash, std::move(i32Node1));
        }

        return i32Node1;
    }

    // todo: error handling
    return nullptr;
}