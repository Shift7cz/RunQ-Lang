#pragma once

#include <vector>
#include <memory>

enum class NodeType {
    Unassigned,

    Func,
    Return,
    I32Literal
};

class Node {
public:
    NodeType type;

    Node(NodeType t) : type(t) {}

    virtual ~Node() = default;
};

class UnassignedNode : public Node { // used for declaring before assignment.
public:
    UnassignedNode() : Node(NodeType::Unassigned) {}
};

class FuncNode : public Node {
public:
    std::string_view identifier;
    std::vector<std::unique_ptr<Node>> body;

    FuncNode() : Node(NodeType::Func) {}
};

class ReturnNode : public Node {
public:
    std::unique_ptr<Node> body;
    ReturnNode() : Node(NodeType::Return) {}
};

class I32LiteralNode : public Node {
public:
    int value;
    I32LiteralNode() : Node(NodeType::I32Literal) {}
};