#pragma once

#include "AstNode.hpp"

class Ast {
public:
    std::unique_ptr<AstNode> root = nullptr;

    void print();
    void add(Ast& otherAst);
    std::string typeToString(Type type);
private:
    void printNode(AstNode* node, int indent);
};