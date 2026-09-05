#include "CodeGen.hpp"
#include <iostream>
#include <string>
#include <string_view>

CodeGen::CodeGen() : llvm("RunQ-Lang_Test") {
}

void CodeGen::compileFunction(FuncNode* node) {
    llvm::Function* fn;

    switch (node->returnType) {
        case TokenType::I32:
            fn = llvm.createFunc(std::string(node->identifier), llvm.i32Type());
            break;
            // todo: other types here
        default:
            // todo: error handling
            return;
    }
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
        case NodeType::VarDeclare: {
            return compileVarDeclare(static_cast<VarDeclareNode*>(node));
        }
        case NodeType::VarLoad: {
            return compileVarLoad(static_cast<VarLoadNode*>(node));
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

llvm::Value * CodeGen::compileVarDeclare(VarDeclareNode *node) {
    llvm::Value* pointer = llvm.createAlloca(static_cast<std::string>(node->identifier), llvm.i32Type()); // todo: other types

    symbolTable[std::string(node->identifier)] = pointer;

    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createStore(val, pointer);
}

llvm::Value * CodeGen::compileVarLoad(VarLoadNode *node) { // todo: look up actual type from symbol table when checker is implemented
    auto* varLoadNode = static_cast<VarLoadNode*>(node);
    return llvm.createLoad(llvm.i32Type(), symbolTable[std::string(node->identifier)]);
}

void CodeGen::compile(Ast& ast, bool generateCompiledFiles) {
    if (!ast.root) return;

    if (ast.root->type == NodeType::Func) {
        compileFunction(static_cast<FuncNode*>(ast.root.get()));
    }

    std::cout << "\n--- LLVM IR Output ---\n";
    llvm.print();

    if (!generateCompiledFiles) return;

    std::cout << "\n--- COMPILING ---\n";
    llvm.emitObjectFile("output.o");
    llvm.emitAssembly("output.s");
    system("clang output.o -o output");
    std::cout << "Executable: ./output" << std::endl;
}
