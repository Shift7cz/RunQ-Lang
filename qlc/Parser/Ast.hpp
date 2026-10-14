#pragma once

#include "Node.hpp"

class Ast {
public:
    std::unique_ptr<Node> root = nullptr;

    void print();
private:
    void printNode(Node* node, int indent);
};