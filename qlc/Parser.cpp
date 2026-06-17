#include "Parser.hpp"

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
    if (!expectAndConsume(TokenType::OpenBrace)) return nullptr;

    auto funcNode = std::make_unique<FuncNode>();
    funcNode->identifier = identifier;

    while (!expect(TokenType::ClosedBrace) && currentToken.type != TokenType::EndOfFile) {
        if (currentToken.type == TokenType::Return) {
            std::unique_ptr<Node> retNode = parseRet();
                funcNode->body.push_back(std::move(retNode));
        }
        else {
        // Todo: other statements
        advance();
        }
    }

    if (!expectAndConsume(TokenType::ClosedBrace)) return nullptr;

    return funcNode;
}

std::unique_ptr<Node> Parser::parseRet() {
    expectAndConsume(TokenType::Return);

    if (expect(TokenType::I32Literal)) {
        auto retNode = std::make_unique<RetIntNode>();

        std::string_view data = currentToken.data;
        int value = 0;
        std::from_chars(data.data(), data.data() + data.size(), value);
        retNode->value = value;

        advance();

        if (!expectAndConsume(TokenType::Semicolon)) {return nullptr;}

        return retNode;
    }
    else {
        return nullptr;
    }
}
