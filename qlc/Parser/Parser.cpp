#include "Parser.hpp"

#include <iostream>
#include <llvm/IR/Argument.h>
#include "../Checker/Logger.hpp"

void Parser::advance() {
    currentToken = lexer.nextToken();
    while (currentToken.type == TokenType::Comment) {
        currentToken = lexer.nextToken();
    }
}

bool Parser::expect(TokenType expected) {
    if (currentToken.type == expected) {
        return true;
    }
    return false;
}

bool Parser::expectAndConsume(TokenType expected, bool logError) {
    if (!expect(expected)) {
        if (logError) Log::error("Expected '" + std::string(tokenTypeToString(expected)) + "', but got '" + currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
        return false;
    }
    advance();
    return true;
}

Type Parser::getType() {
    TokenType token;
    std::string data;
    if (expect(TokenType::SignedInt)
    || expect(TokenType::UnsignedInt)
    || expect(TokenType::F64)
    || expect(TokenType::F32)
    || expect(TokenType::Bool)
    || expect(TokenType::Char)
    || expect(TokenType::Void)) {
        token = currentToken.type;
        data = currentToken.data;
        advance();
    }
    else {
        Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
        advance();
        return {Unknown, 0, false};
    }

    switch (token) {
        case TokenType::F64: return {Float, 64, true}; break; // true in float is convention, doesnt mean anything is just a filler data to make c++ happy
        case TokenType::F32: return {Float, 32, true}; break;
        case TokenType::Bool: return {Bool, 1, false}; break;
        case TokenType::Char: return {Char, 8, true}; break;
        case TokenType::Void: return {Void, 0, false}; break; // false in void is convention
    }

    for (int i = 0; i < data.length(); i++) if (!isdigit(data[i])) Log::error("'" + data + "' isnt valid int count for an int data type", currentToken.line, currentToken.column, currentToken.source); return {Unknown, 0, false}; // checks if data is all digits
    size_t bits = std::stoull(data); // gets the num of digits

    switch (token) {
        case TokenType::SignedInt: return {Int, bits, true}; break;
        case TokenType::UnsignedInt: return {Int, bits, false}; break;
    }
    Log::error("Unexpected error either in your type definition or in compilers parser in getType()", currentToken.line, currentToken.column, currentToken.source);
    return {Unknown, 0, false};
}

Type Parser::makeType(Token token) {
    TokenType tokenType;
    std::string data;
    if (expect(TokenType::SignedInt)
    || expect(TokenType::UnsignedInt)
    || expect(TokenType::F64)
    || expect(TokenType::F32)
    || expect(TokenType::Bool)
    || expect(TokenType::Char)
    || expect(TokenType::Void)) {
        tokenType = token.type;
        data = token.data;
    }
    else {
        Log::error("Unexpected token '"+ token.data + "'", token.line, token.column, token.source);
        return {Unknown, 0, false};
    }

    switch (tokenType) {
        case TokenType::F64: return {Float, 64, true}; break; // true in float is convention, doesnt mean anything is just a filler data to make c++ happy
        case TokenType::F32: return {Float, 32, true}; break;
        case TokenType::Bool: return {Bool, 1, false}; break;
        case TokenType::Char: return {Char, 8, true}; break;
        case TokenType::Void: return {Void, 0, false}; break; // false in void is convention
    }

    for (int i = 0; i < data.length(); i++) if (!isdigit(data[i])) Log::error("'" + data + "' isnt valid int count for an int data type", token.line, token.column, token.source); return {Unknown, 0, false}; // checks if data is all digits
    size_t bits = std::stoull(data); // gets the num of digits

    switch (tokenType) {
        case TokenType::SignedInt: return {Int, bits, true}; break;
        case TokenType::UnsignedInt: return {Int, bits, false}; break;
    }
    Log::error("Unexpected error either in your type definition or in compilers parser in getType()", token.line, token.column, token.source);
    return {Unknown, 0, false};
}

void Parser::parse() {
    advance();

    auto rootNode = std::make_unique<RootNode>();

    rootNode->body = parseBlock(false);

    ast.root = std::move(rootNode);
}

std::vector<std::unique_ptr<Node>> Parser::parseBlock(bool stopAtBrace) {
    std::vector<std::unique_ptr<Node>> block;

    while (!(expect(TokenType::ClosedBrace) && stopAtBrace) && currentToken.type != TokenType::EndOfFile) {
        //std::cout << "parseBlock loop: " << tokenTypeToString(currentToken.type) << " '" << currentToken.data << "'\n";
        if (currentToken.type == TokenType::Return) {
            std::unique_ptr<Node> retNode = parseRet();
            block.push_back(std::move(retNode));
        }
        else if (currentToken.type == TokenType::Let) {
            std::unique_ptr<Node> varNode = parseVarDeclare();
            block.push_back(std::move(varNode));
        }
        else if (currentToken.type == TokenType::If) {
            std::unique_ptr<Node> newIfNode = parseIf();
            block.push_back(std::move(newIfNode));
        }
        else if (currentToken.type == TokenType::Identifier) {
            std::unique_ptr<Node> varNode = parseLoad();
            if (!varNode) return {}; // propagate the parse error
            if (varNode->type == NodeType::FuncLoad) {
                if (!expectAndConsume(TokenType::Semicolon, true)) return {};
            }
            block.push_back(std::move(varNode));
        }
        else if (currentToken.type == TokenType::While) {
            std::unique_ptr<Node> whileNode = parseWhile();
            block.push_back(std::move(whileNode));
        }
        else if (currentToken.type == TokenType::Fn) {
            std::unique_ptr<Node> fnNode = parseFn();
            block.push_back(std::move(fnNode));
        }
        else if  (currentToken.type == TokenType::Extern) {
            std::unique_ptr<Node> externNode = parseExternFn();
            block.push_back(std::move(externNode));
        }
        else if (currentToken.type == TokenType::Entry) {
            //std::cout << "entering fn in entry\n";
            std::unique_ptr<Node> entryNode = parseEntry();
            block.push_back(std::move(entryNode));
        }
        // Todo: other statements
        else {
            Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
            advance();
        }
    }
    return block;
}

std::unique_ptr<Node> Parser::parseFn() {
    if (!expectAndConsume(TokenType::Fn, true)) return nullptr; // move past fn
    auto funcNode = std::make_unique<FuncNode>();

    if (!expect(TokenType::Identifier)) return nullptr;
    std::string identifier = currentToken.data;
    advance(); // move past identifier

    if (!expectAndConsume(TokenType::OpenParen, true)) return nullptr;

    while (!expect(TokenType::ClosedParen)) { //args
        Argument argument;

        //std::cout << "1" << currentToken.data << "\n";
        argument.type = getType();
        //std::cout << "2" << currentToken.data << "\n";

        if (!expect(TokenType::Identifier)) {
            Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
            return nullptr;
        }
        argument.identifier = currentToken.data;
        advance();
        //std::cout << "3" << currentToken.data << "\n";

        funcNode->args.push_back(argument);

        if (expect(TokenType::Comma)) advance();
        else if (!expect(TokenType::ClosedParen)) {
            Log::error("Expected token ',' but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
            return nullptr;
        }
        //std::cout << "4" << currentToken.data << "\n";
    }
    advance();
    //std::cout << "5" << currentToken.data << "\n";

    Type retType;
    if (expectAndConsume(TokenType::Colon, true)) {
        retType = getType();
    }

    if (!expectAndConsume(TokenType::OpenBrace, true)) return nullptr;

    funcNode->identifier = identifier;
    funcNode->returnType = retType;
    funcNode->body = parseBlock(true);

    if (!expectAndConsume(TokenType::ClosedBrace, true)) return nullptr;

    return funcNode;
}

std::unique_ptr<Node> Parser::parseExternFn() {
    if (!expectAndConsume(TokenType::Extern, true)) return nullptr; // move past extern
    if (!expectAndConsume(TokenType::Fn, true)) return nullptr; // move past fn
    auto funcNode = std::make_unique<ExternFuncNode>();

    if (!expect(TokenType::Identifier)) return nullptr;
    std::string identifier = currentToken.data;
    advance(); // move past identifier

    if (!expectAndConsume(TokenType::OpenParen, true)) return nullptr;

    while (!expect(TokenType::ClosedParen)) { //args
        Argument argument;

        //std::cout << "1" << currentToken.data << "\n";
        argument.type = getType();

        if (!expect(TokenType::Identifier)) {
            Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
            advance();
            return nullptr;
        }
        argument.identifier = currentToken.data;
        advance();
        //std::cout << "3" << currentToken.data << "\n";

        funcNode->args.push_back(argument);

        if (expect(TokenType::Comma)) advance();
        else if (!expect(TokenType::ClosedParen)) {
            Log::error("Expected token ',' but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
            return nullptr;
        }
        //std::cout << "4" << currentToken.data << "\n";
    }
    advance();
    //std::cout << "5" << currentToken.data << "\n";

    Type retType;
    if (expectAndConsume(TokenType::Colon, true)) {
        retType = getType();
    }

    if (!expectAndConsume(TokenType::Semicolon, true)) return nullptr;

    funcNode->identifier = identifier;
    funcNode->returnType = retType;

    //std::cout << "retType: " << (int)retType << "\n";

    return funcNode;
}

std::unique_ptr<Node> Parser::parseEntry() {
    //std::cout << "start of entry\n";
    if (!expectAndConsume(TokenType::Entry, true)) return nullptr;

    if (!expect(TokenType::Identifier)) return nullptr;
    std::string identifier = currentToken.data;
    advance(); // move past identifier

    //std::cout << "idnentifier\n";

    if (!expectAndConsume(TokenType::OpenParen, true)) return nullptr;

    // todo: implement parsing parameters

    if (!expectAndConsume(TokenType::ClosedParen, true)) return nullptr;

    Type retType = {Int, 32, true}; // entry has hardcoded return type todo: dynamic return type based on platform?

    //std::cout << "type and params\n";

    if (!expectAndConsume(TokenType::OpenBrace, true)) return nullptr;

    auto funcNode = std::make_unique<FuncNode>();
    funcNode->identifier = identifier;
    funcNode->returnType = retType;
    funcNode->body = parseBlock(true);

    //std::cout << "body\n";

    if (!expectAndConsume(TokenType::ClosedBrace, true)) return nullptr;

    return funcNode;
}

std::unique_ptr<Node> Parser::parseRet() {
    if (!expectAndConsume(TokenType::Return, true)) return nullptr;

    auto retNode = std::make_unique<ReturnNode>();
    retNode->body = parseComparison(); // this now leaves cursor right after the literal

    if (!retNode->body) {
        Log::error("Unexpected error at return", currentToken.line, currentToken.column, currentToken.source);
        return nullptr;
    }
    if (!expectAndConsume(TokenType::Semicolon, true)) return nullptr;

    return retNode;
}

std::unique_ptr<Node> Parser::parseIf() {
    if (!expectAndConsume(TokenType::If, true)) return nullptr;
    auto ifNode = std::make_unique<IfNode>();

    //if (!expect(TokenType::BoolLiteral)) return nullptr;
    //ifNode->condition = parseBoolLiteral();
    ifNode->condition = parseComparison();

    if (!expectAndConsume(TokenType::OpenBrace, true)) return nullptr;
    // todo: figure out a way to remove recursive code here
    ifNode->ifBody = parseBlock(true);
    if (!expectAndConsume(TokenType::ClosedBrace, true)) return nullptr;

    if (expectAndConsume(TokenType::Else, false)) {
        if (!expectAndConsume(TokenType::OpenBrace, true)) return nullptr; // todo: make this work with else if and the 'if something: doCode;'
        ifNode->elseBody = parseBlock(true);
        if (!expectAndConsume(TokenType::ClosedBrace, true)) return nullptr;
    }

    return ifNode;
}

std::unique_ptr<Node> Parser::parseWhile() {
    //std::cout << "entered while\n";
    if (!expectAndConsume(TokenType::While, true)) return nullptr;
    auto whileNode = std::make_unique<WhileNode>();

    //std::cout << "entered condition\n";
    whileNode->condition = parseComparison();

    if (!expectAndConsume(TokenType::OpenBrace, true)) return nullptr;
    //std::cout << "entered body\n";
    // todo: figure out a way to remove recursive code here
    whileNode->body = parseBlock(true);
    if (!expectAndConsume(TokenType::ClosedBrace, true)) return nullptr;
    return whileNode;
}

std::unique_ptr<Node> Parser::parseVarDeclare() {
    if (!expectAndConsume(TokenType::Let, true)) return nullptr;

    bool isMutable = false;
    if (expectAndConsume(TokenType::Tilde, false)) isMutable = true;

    auto varNode = std::make_unique<VarDeclareNode>();
    varNode->isMutable = isMutable;

    varNode->valueType = getType();

    if (expect(TokenType::Identifier)) {
        varNode->identifier = currentToken.data;
        advance();
    }
    else {
        Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
        return nullptr;
    }

    if (!expectAndConsume(TokenType::Equals, true)) return nullptr;

    varNode->body = parseExpresion();

    if (!varNode->body)  {
        Log::error("Unexpected error at variable loading", currentToken.line, currentToken.column, currentToken.source);
        return nullptr;
    }
    if (!expectAndConsume(TokenType::Semicolon, true)) return nullptr;

    return varNode;
}

std::unique_ptr<Node> Parser::parseLoad() {
    if (expect(TokenType::Identifier)) {
        std::string identifier = currentToken.data;
        advance(); // after the identifier

        if (expect(TokenType::OpenParen)) {
            // parse as function
            auto funcNode = std::make_unique<FuncLoadNode>();
            funcNode->identifier = identifier;
            advance();
            // args loop goes here

            while (!expect(TokenType::ClosedParen)) {
                //std::cout << "1" << currentToken.data << std::endl;
                funcNode->args.push_back(parseExpresion());
                //advance();

                //std::cout << "2" << currentToken.data << std::endl;
                if (expect(TokenType::Comma)) advance();
                else if (!expect(TokenType::ClosedParen)) {
                    Log::error("Expected token ',' but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
                    return nullptr;
                }
                //std::cout << "3" << currentToken.data << std::endl;
            }
            advance();
            //if (!expectAndConsume(TokenType::ClosedParen)) return nullptr;
            //if (expect(TokenType::Semicolon)) advance();
            return funcNode;
        }
        // parse as variable
        if (expect(TokenType::Equals)) { // parse as mod
            advance();
            auto varNode = std::make_unique<VarModNode>();
            varNode->identifier = identifier;
            varNode->body = parseExpresion();

            if (!expectAndConsume(TokenType::Semicolon, true)) return nullptr;

            return varNode;
        }
        // parse as load
        auto varNode = std::make_unique<VarLoadNode>();
        varNode->identifier = identifier;
        return varNode;
    }
    Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
    return nullptr;
}

std::unique_ptr<Node> Parser::parseVarMod() { // NOT IN USE !!!
    if (!expect(TokenType::Identifier))  {
        Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
        return nullptr;
    }

    auto varNode = std::make_unique<VarModNode>();
    varNode->identifier = currentToken.data;
    advance();

    if (!expectAndConsume(TokenType::Equals, true)) return nullptr;

    varNode->body = parseExpresion();

    if (!expectAndConsume(TokenType::Semicolon, true)) return nullptr;

    return varNode;
}

std::unique_ptr<Node> Parser::parseIntLiteral() {
    if (expect(TokenType::IntLiteral)) {
        auto intNode = std::make_unique<IntLiteralNode>();
        intNode->value = std::atoi(currentToken.data.data());

        advance();

        for (int i = 0; i < currentToken.data.length(); i++) if (!isdigit(currentToken.data[i])) { // checks if data is all digits
            Log::error("'" + currentToken.data + "' isnt valid int count for an int data type", currentToken.line, currentToken.column, currentToken.source);
            return nullptr;
        }
        size_t bits = std::stoull(currentToken.data); // gets the num of digits

        switch (currentToken.type) {
            case TokenType::SignedInt: intNode->type = {Int, bits, true};
            case TokenType::UnsignedInt: intNode->type = {Int, bits, false};
            default:
                Log::error("Unexpected error either in your type definition or in compilers parser in parseIntLiteral()", currentToken.line, currentToken.column, currentToken.source);
                return nullptr;
        }

        return intNode;
    }
    Log::error("Expected int literal but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
    return nullptr;
}

std::unique_ptr<Node> Parser::parseFloatLiteral() { //todo: f32/f64 handling
    if (expect(TokenType::FloatLiteral)) {
        auto floatNode = std::make_unique<FloatLiteralNode>();
        floatNode->value = std::atof(currentToken.data.data());

        advance();

        return floatNode;
    }
    Log::error("Expected float literal but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
    return nullptr;
}

std::unique_ptr<Node> Parser::parseBoolLiteral() {
    if (expect(TokenType::BoolLiteral)) {
        auto boolNode = std::make_unique<BoolLiteralNode>();
        if (currentToken.data == "true") boolNode->value = true;
        else if (currentToken.data == "false") boolNode->value = false;
        else {
            Log::error("Expected bool literal (true/false) but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
            return nullptr;
        }

        advance();

        return boolNode;
    }
    Log::error("Expected bool literal (true/false) but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
    return nullptr;
}

std::unique_ptr<Node> Parser::parseCharLiteral() {
    if (expect(TokenType::CharLiteral)) {
        auto boolNode = std::make_unique<CharLiteralNode>();
        boolNode->value = currentToken.data[0]; // todo: escape sequence parsing

        advance();

        return boolNode;
    }
    Log::error("Expected bool literal (true/false) but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
    return nullptr;
}

std::unique_ptr<Node> Parser::parsePrimary() {
    if (expect(TokenType::IntLiteral))   return parseIntLiteral();
    if (expect(TokenType::FloatLiteral)) return parseFloatLiteral();
    if (expect(TokenType::BoolLiteral))  return parseBoolLiteral();
    if (expect(TokenType::CharLiteral))  return parseCharLiteral();
    if (expect(TokenType::Identifier))   return parseLoad();
    Log::error("Expected any literal but got '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
    return nullptr;
}

std::unique_ptr<Node> Parser::parseMultiplicative() {
    auto left = parsePrimary();
    while (expect(TokenType::Star) || expect(TokenType::Slash)) {
        TokenType op = currentToken.type;
        advance();
        auto right = parsePrimary();
        auto node = std::make_unique<MathOperatorNode>();
        node->operatorType = op;
        node->operand1 = std::move(left);
        node->operand2 = std::move(right);
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<Node> Parser::parseComparison() { // todo: comparisons with floats
    if (expect(TokenType::BoolLiteral)) {
        auto boolNode = parseBoolLiteral();
        return boolNode;
    }

    // if (expect(TokenType::Identifier)) {
    //     auto varNode = parseVarLoad();
    //     return varNode;
    // }

    auto compareNode = std::make_unique<CompareNode>();

    if (expect(TokenType::IntLiteral)) {
        std::unique_ptr<Node> operand1 = parseExpresion();
        compareNode->operand1 = std::move(operand1);
    }
    else if (expect(TokenType::CharLiteral)) {
        compareNode->operand1 = parseCharLiteral();
    }
    else if (expect(TokenType::Identifier)) {
        std::unique_ptr<Node> operand1 = parseExpresion();
        compareNode->operand1 = std::move(operand1);
    }
    else {
        Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
        return nullptr;
    }

    if (expect(TokenType::IsEqualTo)) compareNode->compareType = currentToken.type;
    else if (expect(TokenType::NotEqualTo)) compareNode->compareType = currentToken.type;
    else if (expect(TokenType::LessThan)) compareNode->compareType = currentToken.type;
    else if (expect(TokenType::GreaterThan)) compareNode->compareType = currentToken.type;
    else if (expect(TokenType::LessOrEqual)) compareNode->compareType = currentToken.type;
    else if (expect(TokenType::GreaterOrEqual)) compareNode->compareType = currentToken.type;
    else {
        return std::move(compareNode->operand1);
    }

    advance();

    if (expect(TokenType::IntLiteral) || expect(TokenType::CharLiteral)) {
        std::unique_ptr<Node> operand2 = parseExpresion();
        compareNode->operand2 = std::move(operand2);
        return compareNode;
    }
    if (expect(TokenType::Identifier)) {
        std::unique_ptr<Node> operand2 = parseExpresion();
        compareNode->operand2 = std::move(operand2);
        return compareNode;
    }
    Log::error("Unexpected token '"+ currentToken.data + "'", currentToken.line, currentToken.column, currentToken.source);
    return nullptr;
}

std::unique_ptr<Node> Parser::parseExpresion() {
    auto left = parseMultiplicative();
    while (expect(TokenType::Plus) || expect(TokenType::Dash)) {
        TokenType op = currentToken.type;
        advance();
        auto right = parseMultiplicative();
        auto node = std::make_unique<MathOperatorNode>();
        node->operatorType = op;
        node->operand1 = std::move(left);
        node->operand2 = std::move(right);
        left = std::move(node);
    }
    return left;
}