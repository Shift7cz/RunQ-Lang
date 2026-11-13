#pragma once
#include <string>
#include <vector>
#include "../Type/Type.hpp"
#include "../Lexer/Token.hpp"

enum AslNodeType {
    Unassigned,

    // functions
    Func,
    ExternFunc,
    Return,

    // control flow
    Marker,
    Jump,
    CondJump,

    // data + literals
    MakeImut,
    CreateMut,
    StoreMut,
    CallFn,

    // ops
    MathInstr,
    CompareInstr,
};

class AslNode {
public:
    AslNodeType type;

    AslNode(AslNodeType t)  : type(t) {};

    virtual ~AslNode() = default;
};

class UnassignedAslNode : public AslNode {
public:
    UnassignedAslNode(AslNodeType t) : AslNode(t) {};
};

struct AslArgument {
    int identifier;
    Type type;
};

class FuncAslNode : public AslNode {
public:
    int identifier;
    std::vector<AslArgument> args;
    Type returnType;

    FuncAslNode(AslNodeType t) : AslNode(t) {};
};

class ExternFuncAslNode : public AslNode {
public:
    int identifier;
    std::vector<AslArgument> args;
    Type returnType;
    ExternFuncAslNode(AslNodeType t) : AslNode(t) {};
};

class ReturnAslNode : public AslNode {
public:
    int Reg;

    ReturnAslNode(AslNodeType t) : AslNode(t) {};
};

class MarkerNode : public AslNode {
public:
    int identifier;

    MarkerNode(AslNodeType t) : AslNode(t) {};
};

class JumpNode : public AslNode {
public:
    int identifier;

    JumpNode(AslNodeType t) : AslNode(t) {};
};

class CondJumpNode : public AslNode {
public:
    int condReg;
    int identifierIfTrue;
    int identifierIfFalse;

    CondJumpNode(AslNodeType t) : AslNode(t) {};
};

class MakeImutIntNode : public AslNode {
public:
    int identifier;
    Type dataType;
    std::string data; // string bcoz it cna handle any data and even u2048 sized data
};

class CreateMutNode : public AslNode {
public:
    int identifier;
    Type dataType;

    CreateMutNode(AslNodeType t) : AslNode(t) {};
};

class StoreMutNode : public AslNode {
public:
    int identifierMut;
    int identifierSource;

    StoreMutNode(AslNodeType t) : AslNode(t) {};
};

class CallFnNode : public AslNode {
public:
    int identifier;
    std::vector<AslArgument> args;

    CallFnNode(AslNodeType t) : AslNode(t) {};
};

class MathInstrNode : public AslNode {
public:
    int identifierTarget;
    int identifierOp1;
    int identifierOp2;
    TokenType op;

    MathInstrNode(AslNodeType t) : AslNode(t) {};
};

class CompareInstrNode : public AslNode {
public:
    int identifierTarget;
    int identifierOp1;
    int identifierOp2;
    TokenType op;

    CompareInstrNode(AslNodeType t) : AslNode(t) {};
};