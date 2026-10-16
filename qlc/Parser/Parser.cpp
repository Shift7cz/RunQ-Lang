#include "Parser.hpp"

#include <iostream>
#include <llvm/IR/Argument.h>

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

    auto rootNode = std::make_unique<RootNode>();

    rootNode->body = parseBlock(false);

    ast.root = std::move(rootNode);
}

std::vector<std::unique_ptr<Node>> Parser::parseBlock(bool stopAtBrace) {
    std::vector<std::unique_ptr<Node>> block;

    while (!(expect(TokenType::ClosedBrace) && stopAtBrace) && currentToken.type != TokenType::EndOfFile) { // todo: error handling for end of file
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
            std::unique_ptr<Node> varNode = parseVarMod();
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
            std::cout << "entering fn in entry\n";
            std::unique_ptr<Node> entryNode = parseEntry();
            block.push_back(std::move(entryNode));
        }
        // Todo: other statements
        else {
            advance();
        }
    }
    return block;
}

std::unique_ptr<Node> Parser::parseFn() {
    expectAndConsume(TokenType::Fn); // move past fn
    auto funcNode = std::make_unique<FuncNode>();

    if (!expect(TokenType::Identifier)) return nullptr;
    std::string_view identifier = currentToken.data;
    advance(); // move past identifier

    if (!expectAndConsume(TokenType::OpenParen)) return nullptr; // todo: error handling

    while (!expect(TokenType::ClosedParen)) { //args
        Argument argument;

        //std::cout << "1" << currentToken.data << "\n";
        if (expect(TokenType::I32)
            || expect(TokenType::I8)
            || expect(TokenType::F64)
            || expect(TokenType::Bool)
            || expect(TokenType::Char)) {
            argument.type = currentToken.type;
            advance();
            }
        else return nullptr;
        //std::cout << "2" << currentToken.data << "\n";

        if (!expect(TokenType::Identifier)) return nullptr; // todo: error handling
        argument.identifier = currentToken.data;
        advance();
        //std::cout << "3" << currentToken.data << "\n";

        funcNode->args.push_back(argument);

        if (expect(TokenType::Comma)) advance(); // todo: error handling
        //std::cout << "4" << currentToken.data << "\n";
    }
    advance();
    //std::cout << "5" << currentToken.data << "\n";

    TokenType retType = TokenType::Unknown;
    if (expectAndConsume(TokenType::Colon)) {
        // moves into return type if return type is available
        if (expect(TokenType::I32)
            || expect(TokenType::I8)
            || expect(TokenType::F64)
            || expect(TokenType::Bool)
            || expect(TokenType::Char)) {
            retType = currentToken.type;
            advance();
        }
        else {
            // todo: error handling
            return nullptr;
        }
    }

    if (!expectAndConsume(TokenType::OpenBrace)) return nullptr;

    funcNode->identifier = identifier;
    funcNode->returnType = retType;
    funcNode->body = parseBlock(true);

    if (!expectAndConsume(TokenType::ClosedBrace)) return nullptr;

    return funcNode;
}

std::unique_ptr<Node> Parser::parseExternFn() {
    expectAndConsume(TokenType::Extern); // move past extern
    expectAndConsume(TokenType::Fn); // move past fn
    auto funcNode = std::make_unique<ExternFuncNode>();

    if (!expect(TokenType::Identifier)) return nullptr;
    std::string_view identifier = currentToken.data;
    advance(); // move past identifier

    if (!expectAndConsume(TokenType::OpenParen)) return nullptr; // todo: error handling

    while (!expect(TokenType::ClosedParen)) { //args
        Argument argument;

        //std::cout << "1" << currentToken.data << "\n";
        if (expect(TokenType::I32)
            || expect(TokenType::I8)
            || expect(TokenType::F64)
            || expect(TokenType::Bool)
            || expect(TokenType::Char)) {
            argument.type = currentToken.type;
            advance();
            }
        else return nullptr;
        //std::cout << "2" << currentToken.data << "\n";

        if (!expect(TokenType::Identifier)) return nullptr; // todo: error handling
        argument.identifier = currentToken.data;
        advance();
        //std::cout << "3" << currentToken.data << "\n";

        funcNode->args.push_back(argument);

        if (expect(TokenType::Comma)) advance(); // todo: error handling
        //std::cout << "4" << currentToken.data << "\n";
    }
    advance();
    //std::cout << "5" << currentToken.data << "\n";

    TokenType retType = TokenType::Unknown;
    if (expectAndConsume(TokenType::Colon)) {
        // moves into return type if return type is available
        if (expect(TokenType::I32)
            || expect(TokenType::I8)
            || expect(TokenType::F64)
            || expect(TokenType::Bool)
            || expect(TokenType::Char)) {
            retType = currentToken.type;
            advance();
        }
        else {
            // todo: error handling
            return nullptr;
        }
    }

    if (!expectAndConsume(TokenType::Semicolon)) return nullptr;

    funcNode->identifier = identifier;
    funcNode->returnType = retType;

    //std::cout << "retType: " << (int)retType << "\n";

    return funcNode;
}

std::unique_ptr<Node> Parser::parseEntry() {
    //std::cout << "start of entry\n";
    expectAndConsume(TokenType::Entry);

    if (!expect(TokenType::Identifier)) return nullptr;
    std::string_view identifier = currentToken.data;
    advance(); // move past identifier

    //std::cout << "idnentifier\n";

    if (!expectAndConsume(TokenType::OpenParen)) return nullptr;

    // todo: implement parsing parameters

    if (!expectAndConsume(TokenType::ClosedParen)) return nullptr;

    TokenType retType = TokenType::I32; // entry has hardcoded return type

    //std::cout << "type and params\n";

    if (!expectAndConsume(TokenType::OpenBrace)) return nullptr;

    auto funcNode = std::make_unique<FuncNode>();
    funcNode->identifier = identifier;
    funcNode->returnType = retType;
    funcNode->body = parseBlock(true);

    //std::cout << "body\n";

    if (!expectAndConsume(TokenType::ClosedBrace)) return nullptr;

    return funcNode;
}

std::unique_ptr<Node> Parser::parseRet() {
    if (!expectAndConsume(TokenType::Return)) return nullptr;

    auto retNode = std::make_unique<ReturnNode>();
    retNode->body = parseComparison(); // this now leaves cursor right after the literal

    if (!retNode->body) return nullptr;
    if (!expectAndConsume(TokenType::Semicolon)) return nullptr;

    return retNode;
}

std::unique_ptr<Node> Parser::parseIf() {
    if (!expectAndConsume(TokenType::If)) return nullptr; // todo: error handling
    auto ifNode = std::make_unique<IfNode>();

    //if (!expect(TokenType::BoolLiteral)) return nullptr;
    //ifNode->condition = parseBoolLiteral();
    ifNode->condition = parseComparison();

    if (!expectAndConsume(TokenType::OpenBrace)) return nullptr;
    // todo: figure out a way to remove recursive code here
    ifNode->ifBody = parseBlock(true);
    if (!expectAndConsume(TokenType::ClosedBrace)) return nullptr;

    if (expectAndConsume(TokenType::Else)) {
        if (!expectAndConsume(TokenType::OpenBrace)) return nullptr; // todo: make this work with else if and the 'if (something) doCode;'
        ifNode->elseBody = parseBlock(true);
        if (!expectAndConsume(TokenType::ClosedBrace)) return nullptr;
    }

    return ifNode;
}

std::unique_ptr<Node> Parser::parseWhile() {
    //std::cout << "entered while\n";
    if (!expectAndConsume(TokenType::While)) return nullptr; // todo: error handling
    auto whileNode = std::make_unique<WhileNode>();

    //std::cout << "entered condition\n";
    whileNode->condition = parseComparison();

    if (!expectAndConsume(TokenType::OpenBrace)) return nullptr; // todo: error handling
    //std::cout << "entered body\n";
    // todo: figure out a way to remove recursive code here
    whileNode->body = parseBlock(true);
    if (!expectAndConsume(TokenType::ClosedBrace)) return nullptr;
    return whileNode;
}

std::unique_ptr<Node> Parser::parseVarDeclare() {
    if (!expectAndConsume(TokenType::Let)) return nullptr;

    bool isMutable = false;
    if (expectAndConsume(TokenType::Tilde)) isMutable = true;

    auto varNode = std::make_unique<VarDeclareNode>();
    varNode->isMutable = isMutable;

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

std::unique_ptr<Node> Parser::parseLoad() {
    if (expect(TokenType::Identifier)) {
        std::string_view identifier = currentToken.data;
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
                //std::cout << "3" << currentToken.data << std::endl;
            }
            advance();
            //if (!expectAndConsume(TokenType::ClosedParen)) return nullptr;
            return funcNode;
        }
        // parse as variable
        auto varNode = std::make_unique<VarLoadNode>();
        varNode->identifier = identifier;
        return varNode;
    }

    // todo: error handling
    return nullptr;
}

std::unique_ptr<Node> Parser::parseVarMod() {
    if (!expect(TokenType::Identifier)) return nullptr; // todo: error handling

    auto varNode = std::make_unique<VarModNode>();
    varNode->identifier = currentToken.data;
    advance();

    if (!expectAndConsume(TokenType::Equals)) return nullptr;

    varNode->body = parseExpresion();

    return varNode;
}

std::unique_ptr<Node> Parser::parseIntLiteral() {
    if (expect(TokenType::IntLiteral)) {
        auto intNode = std::make_unique<IntLiteralNode>();
        intNode->value = std::atoi(currentToken.data.data());

        advance();
        if (expect(TokenType::I8)) {
            intNode->type = currentToken.type;
            advance();
        }
        // todo: other int types
        else intNode->type = TokenType::I32;

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

std::unique_ptr<Node> Parser::parsePrimary() {
    if (expect(TokenType::IntLiteral))   return parseIntLiteral();
    if (expect(TokenType::FloatLiteral)) return parseFloatLiteral();
    if (expect(TokenType::BoolLiteral))  return parseBoolLiteral();
    if (expect(TokenType::CharLiteral))  return parseCharLiteral();
    if (expect(TokenType::Identifier))   return parseLoad();
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
    else if (expect(TokenType::Identifier)) {
        std::unique_ptr<Node> operand1 = parseExpresion();
        compareNode->operand1 = std::move(operand1);
    }
    else return nullptr; // todo: error handling

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

    if (expect(TokenType::IntLiteral)) {
        std::unique_ptr<Node> operand2 = parseExpresion();
        compareNode->operand2 = std::move(operand2);
        return compareNode;
    }
    if (expect(TokenType::Identifier)) {
        std::unique_ptr<Node> operand2 = parseExpresion();
        compareNode->operand2 = std::move(operand2);
        return compareNode;
    }

    // todo: error handling
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