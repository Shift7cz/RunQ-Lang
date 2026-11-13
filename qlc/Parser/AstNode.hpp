#pragma once

#include <vector>
#include <memory>
#include "../Lexer/Token.hpp"
#include "../Type/Type.hpp"

enum class AstNodeType {
    Unassigned,

    Root, // the main node under which the AST lives

    Func,
    ExternFunc,
    Return,
    If, // contains else as optional data
    While,
    VarDeclare,
    VarLoad,
    FuncLoad,
    VarMod,
    IntLiteral,
    FloatLiteral,
    BoolLiteral,
    CharLiteral,
    MathOperator,
    Compare,
};

class AstNode {
public:
    AstNodeType type;

    AstNode(AstNodeType t) : type(t) {}

    virtual ~AstNode() = default;
};

class RootNode : public AstNode {
public:
    std::vector<std::unique_ptr<AstNode>> body;

    RootNode() : AstNode(AstNodeType::Root) {}
};

class UnassignedAstNode : public AstNode { // used for declaring before assignment.
public:
    UnassignedAstNode() : AstNode(AstNodeType::Unassigned) {}
};

struct AstArgument {
    std::string identifier;
    Type type;
};

class FuncAstNode : public AstNode {
public:
    std::string identifier;
    std::vector<AstArgument> args;
    Type returnType;
    std::vector<std::unique_ptr<AstNode>> body;

    FuncAstNode() : AstNode(AstNodeType::Func) {}
};

class ExternFuncAstNode : public AstNode {
public:
    std::string identifier;
    std::vector<AstArgument> args;
    Type returnType; // only section types allowed

    ExternFuncAstNode() : AstNode(AstNodeType::ExternFunc) {}
};

class ReturnAstNode : public AstNode {
public:
    std::unique_ptr<AstNode> body;
    ReturnAstNode() : AstNode(AstNodeType::Return) {}
};

class IfNode : public AstNode {
public:
    std::unique_ptr<AstNode> condition;

    std::vector<std::unique_ptr<AstNode>> ifBody;
    std::vector<std::unique_ptr<AstNode>> elseBody;

    IfNode() : AstNode(AstNodeType::If) {}
};

class WhileNode : public AstNode {
public:
    std::unique_ptr<AstNode> condition;

    std::vector<std::unique_ptr<AstNode>> body;

    WhileNode() : AstNode(AstNodeType::While) {}
};

class VarDeclareNode : public AstNode {
public:
    std::unique_ptr<AstNode> body;
    std::string identifier;
    bool isMutable;
    Type valueType; // the type of the variable, only types allowed

    VarDeclareNode() : AstNode(AstNodeType::VarDeclare) {}
};

class VarLoadNode : public AstNode {
public:
    std::string identifier;

    VarLoadNode() : AstNode(AstNodeType::VarLoad) {}
};

class FuncLoadNode : public AstNode {
public:
    std::string identifier;
    std::vector<std::unique_ptr<AstNode>> args;

    FuncLoadNode() : AstNode(AstNodeType::FuncLoad) {}
};

class VarModNode : public AstNode {
public:
    std::string identifier;
    std::unique_ptr<AstNode> body;

    VarModNode() : AstNode(AstNodeType::VarMod) {}
};

class IntLiteralNode : public AstNode { // todo: same error as with numbers in AslNode.hpp
public:
    int value;
    Type type;
    IntLiteralNode() : AstNode(AstNodeType::IntLiteral) {}
};

class FloatLiteralNode : public AstNode { // todo: add typing when f32 is a thing as in int
public:
    double value;
    FloatLiteralNode() : AstNode(AstNodeType::FloatLiteral) {}
};

class BoolLiteralNode : public AstNode {
public:
    bool value;
    BoolLiteralNode() : AstNode(AstNodeType::BoolLiteral) {}
};

class CharLiteralNode : public AstNode {
public:
    char value;
    CharLiteralNode() : AstNode(AstNodeType::CharLiteral) {}
};

class MathOperatorNode : public AstNode {
public:
    TokenType operatorType; // only section math operators are allowed!
    std::unique_ptr<AstNode> operand1;
    std::unique_ptr<AstNode> operand2;

    MathOperatorNode() : AstNode(AstNodeType::MathOperator) {}
};

class CompareNode : public AstNode {
public:
    TokenType compareType; // only section compressions allowed
    std::unique_ptr<AstNode> operand1;
    std::unique_ptr<AstNode> operand2;

    CompareNode() : AstNode(AstNodeType::Compare) {}
};