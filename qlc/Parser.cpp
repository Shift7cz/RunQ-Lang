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

    auto retNode = std::make_unique<ReturnNode>();
    retNode->body = parseExpresion(); // this now leaves cursor right after the literal

    if (!retNode->body) return nullptr;
    if (!expectAndConsume(TokenType::Semicolon)) return nullptr;

    return retNode;
}

std::unique_ptr<Node> Parser::parseVarDeclare() {
    if (!expectAndConsume(TokenType::Let)) return nullptr;

    auto varNode = std::make_unique<VarDeclareNode>();

    if (expect(TokenType::I32) || expect(TokenType::I8) || expect(TokenType::F64) || expect(TokenType::Bool) || expect(TokenType::Char)) {
        varNode->valueType = currentToken.type;
    }
    else return nullptr;

    advance();

    if (expect(TokenType::Identifier)) {
        varNode->identifier = currentToken.data;
        advance();
    }
    else return nullptr; // todo: error handling

    if (!expectAndConsume(TokenType::Equals)) return nullptr;

    varNode->body = parseExpresion();

    if (!varNode->body) return nullptr;
    if (!expectAndConsume(TokenType::Semicolon)) return nullptr;

    return varNode;
}

std::unique_ptr<Node> Parser::parseVarLoad() {
    if (expect(TokenType::Identifier)) {
        auto varNode = std::make_unique<VarLoadNode>();
        varNode->identifier = currentToken.data;

        advance();

        return varNode;
    }

    // todo: error handling
    return nullptr;
}

std::unique_ptr<Node> Parser::parseIntLiteral() {
    if (expect(TokenType::IntLiteral)) {
        auto intNode = std::make_unique<IntLiteralNode>();
        intNode->value = std::atoi(currentToken.data.data());

        advance();

        return intNode;
    }

    // todo: error handling
    return nullptr;
}

std::unique_ptr<Node> Parser::parseFloatLiteral() {
    if (expect(TokenType::FloatLiteral)) {
        auto floatNode = std::make_unique<FloatLiteralNode>();
        floatNode->value = std::atof(currentToken.data.data());

        advance();

        return floatNode;
    }

    // todo: error handling
    return nullptr;
}

std::unique_ptr<Node> Parser::parseBoolLiteral() {
    if (expect(TokenType::BoolLiteral)) {
        auto boolNode = std::make_unique<BoolLiteralNode>();
        if (currentToken.data == "true") boolNode->value = true;
        else if (currentToken.data == "false") boolNode->value = false;
        else return nullptr; // todo: error handling

        advance();

        return boolNode;
    }

    // todo: error handling
    return nullptr;
}

std::unique_ptr<Node> Parser::parseCharLiteral() {
    if (expect(TokenType::CharLiteral)) {
        auto boolNode = std::make_unique<CharLiteralNode>();
        boolNode->value = currentToken.data[0]; // todo: escape sequence parsing

        advance();

        return boolNode;
    }

    // todo: error handling
    return nullptr;
}

std::unique_ptr<Node> Parser::parseMathOperator(TokenType opType, std::unique_ptr<Node> operand1) {
    auto mathOpNode = std::make_unique<MathOperatorNode>();
    mathOpNode->operatorType = opType;

    advance();

    if (expect(TokenType::IntLiteral)) {
        std::unique_ptr<Node> operand2 = parseExpresion(); // todo: Longer math equations will translate, but they wont compile in the correct order. fix.

        mathOpNode->operand1 = std::move(operand1);
        mathOpNode->operand2 = std::move(operand2);

        return mathOpNode;
    }
    if (expect(TokenType::Identifier)) {
        std::unique_ptr<Node> operand2 = parseExpresion();

        mathOpNode->operand1 = std::move(operand1);
        mathOpNode->operand2 = std::move(operand2);

        return mathOpNode;
    }

    // todo: error handling
    return nullptr;
}


std::unique_ptr<Node> Parser::parseExpresion() {
    if (expect(TokenType::IntLiteral)) {
        std::unique_ptr<Node> intNode1 = parseIntLiteral();

        if (expect(TokenType::Plus)) {
            return parseMathOperator(TokenType::Plus, std::move(intNode1));
        }
        if (expect(TokenType::Dash)) {
            return parseMathOperator(TokenType::Dash, std::move(intNode1));
        }
        if (expect(TokenType::Star)) {
            return parseMathOperator(TokenType::Star, std::move(intNode1));
        }
        if (expect(TokenType::Slash)) {
            return parseMathOperator(TokenType::Slash, std::move(intNode1));
        }

        return intNode1;
    }
    if (expect(TokenType::Identifier)) {
        std::unique_ptr<Node> varLoadNode1 = parseVarLoad();

        if (expect(TokenType::Plus)) {
            return parseMathOperator(TokenType::Plus, std::move(varLoadNode1));
        }
        if (expect(TokenType::Dash)) {
            return parseMathOperator(TokenType::Dash, std::move(varLoadNode1));
        }
        if (expect(TokenType::Star)) {
            return parseMathOperator(TokenType::Star, std::move(varLoadNode1));
        }
        if (expect(TokenType::Slash)) {
            return parseMathOperator(TokenType::Slash, std::move(varLoadNode1));
        }

        return varLoadNode1;
    }
    if (expect(TokenType::FloatLiteral)) {
        std::unique_ptr<Node> floatNode1 = parseFloatLiteral();

        if (expect(TokenType::Plus)) {
            return parseMathOperator(TokenType::Plus, std::move(floatNode1));
        }
        if (expect(TokenType::Dash)) {
            return parseMathOperator(TokenType::Dash, std::move(floatNode1));
        }
        if (expect(TokenType::Star)) {
            return parseMathOperator(TokenType::Star, std::move(floatNode1));
        }
        if (expect(TokenType::Slash)) {
            return parseMathOperator(TokenType::Slash, std::move(floatNode1));
        }

        return floatNode1;
    }
    if (expect(TokenType::BoolLiteral)) { // todo: bool operations here?
        std::unique_ptr<Node> boolNode1 = parseBoolLiteral();

        if (expect(TokenType::Plus)) {
            return parseMathOperator(TokenType::Plus, std::move(boolNode1));
        }
        if (expect(TokenType::Dash)) {
            return parseMathOperator(TokenType::Dash, std::move(boolNode1));
        }
        if (expect(TokenType::Star)) {
            return parseMathOperator(TokenType::Star, std::move(boolNode1));
        }
        if (expect(TokenType::Slash)) {
            return parseMathOperator(TokenType::Slash, std::move(boolNode1));
        }

        return boolNode1;
    }
    if (expect(TokenType::CharLiteral)) {
        std::unique_ptr<Node> charNode1 = parseCharLiteral(); // todo: char operations? here?

        if (expect(TokenType::Plus)) {
            return parseMathOperator(TokenType::Plus, std::move(charNode1));
        }
        if (expect(TokenType::Dash)) {
            return parseMathOperator(TokenType::Dash, std::move(charNode1));
        }
        if (expect(TokenType::Star)) {
            return parseMathOperator(TokenType::Star, std::move(charNode1));
        }
        if (expect(TokenType::Slash)) {
            return parseMathOperator(TokenType::Slash, std::move(charNode1));
        }

        return charNode1;
    }

    // todo: error handling
    return nullptr;
}