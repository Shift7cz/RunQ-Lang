#pragma once

#include <vector>
#include <memory>
#include "Token.hpp"

enum class NodeType {
    Unassigned,

    Func,
    Return,
    If, // contains else as opional data
    VarDeclare,
    VarLoad,
    IntLiteral,
    FloatLiteral,
    BoolLiteral,
    CharLiteral,
    MathOperator,
    Compare,
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
    TokenType returnType; // only section types allowed.

    FuncNode() : Node(NodeType::Func) {}
};

class ReturnNode : public Node {
public:
    std::unique_ptr<Node> body;
    ReturnNode() : Node(NodeType::Return) {}
};

class IfNode : public Node {
public:
    std::unique_ptr<Node> condition;

    std::vector<std::unique_ptr<Node>> ifBody;
    std::vector<std::unique_ptr<Node>> elseBody;

    IfNode() : Node(NodeType::If) {}
};

class VarDeclareNode : public Node {
public:
    std::unique_ptr<Node> body;
    std::string_view identifier;
    TokenType valueType; // the type of the variable, only types allowed

    VarDeclareNode() : Node(NodeType::VarDeclare) {}
};

class VarLoadNode : public Node {
public:
    std::string_view identifier;

    VarLoadNode() : Node(NodeType::VarLoad) {}
};

class IntLiteralNode : public Node {
public:
    int value;
    IntLiteralNode() : Node(NodeType::IntLiteral) {}
};

class FloatLiteralNode : public Node {
public:
    double value;
    FloatLiteralNode() : Node(NodeType::FloatLiteral) {}
};

class BoolLiteralNode : public Node {
public:
    bool value;
    BoolLiteralNode() : Node(NodeType::BoolLiteral) {}
};

class CharLiteralNode : public Node {
public:
    char value;
    CharLiteralNode() : Node(NodeType::CharLiteral) {}
};

class MathOperatorNode : public Node {
public:
    TokenType operatorType; // only section math operators are allowed!
    std::unique_ptr<Node> operand1;
    std::unique_ptr<Node> operand2;

    MathOperatorNode() : Node(NodeType::MathOperator) {}
};

class CompareNode : public Node {
public:
    TokenType compareType; // only section compressions allowed
    std::unique_ptr<Node> operand1;
    std::unique_ptr<Node> operand2;

    CompareNode() : Node(NodeType::Compare) {}
};