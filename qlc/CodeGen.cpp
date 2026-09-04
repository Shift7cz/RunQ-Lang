#include "CodeGen.hpp"
#include <iostream>
#include <string>
#include <string_view>

CodeGen::CodeGen() : llvm("RunQ-Lang_Test") {
}

void CodeGen::compileFunction(FuncNode* node) {
    llvm::Function* fn = llvm.createFunc(std::string(node->identifier), llvm.i32Type());
    llvm.createBlock("entry", fn);

    for (const auto& statement : node->body) {
        compileExpression(statement.get());
    }
}

llvm::Value* CodeGen::compileExpression(Node* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::Return: {
            return compileReturn(static_cast<ReturnNode*>(node));
        }
        case NodeType::I32Literal: {
            auto* literal = static_cast<I32LiteralNode*>(node);
            return llvm.createI32Literal(literal->value);
        }
        case NodeType::MathOperator: {
            auto* mathOp = static_cast<MathOperatorNode*>(node);
            llvm::Value* left = compileExpression(mathOp->operand1.get());
            llvm::Value* right = compileExpression(mathOp->operand2.get());

            switch (mathOp->operatorType) {
                case TokenType::Plus:  return llvm.createPlus(left, right);
                case TokenType::Dash:  return llvm.createMinus(left, right);
                case TokenType::Star:  return llvm.createMultiply(left, right);
                case TokenType::Slash: return llvm.createDevide(left, right);
                default: return nullptr;
            }
        }
        default:
            std::cerr << "Unknown node type in CodeGen" << std::endl;
            return nullptr;
    }
}

llvm::Value* CodeGen::compileReturn(ReturnNode* node) {
    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createReturn(val);
}

void CodeGen::generate(Ast& ast) {
    if (!ast.root) return;

    if (ast.root->type == NodeType::Func) {
        compileFunction(static_cast<FuncNode*>(ast.root.get()));
    }

    std::cout << "\n--- LLVM IR Output ---\n";
    llvm.print();
}