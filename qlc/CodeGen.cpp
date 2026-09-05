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
        case NodeType::IntLiteral: {
            auto* literal = static_cast<IntLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 32);
        }
        case NodeType::FloatLiteral: {
            auto* literal = static_cast<FloatLiteralNode*>(node);
            return llvm.createF64Literal(literal->value); // todo: handling of f32 and potentially f128 later same as in ints
        }
        case NodeType::BoolLiteral: {
            auto* literal = static_cast<BoolLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 1, false); // i1 has to actually be u1, otherwise its either 0 or -1
        }
        case NodeType::CharLiteral: {
            auto* literal = static_cast<CharLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 8);
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
    llvm::Value* pointer;
    switch (node->valueType) {
        case TokenType::I32:
            pointer = llvm.createAlloca(static_cast<std::string>(node->identifier), llvm.i32Type());
            break;
        case TokenType::I8:
            pointer = llvm.createAlloca(static_cast<std::string>(node->identifier), llvm.i32Type()); // todo: change when literal typing is added
            break;
        case TokenType::F64:
            pointer = llvm.createAlloca(static_cast<std::string>(node->identifier), llvm.f64Type());
            break;
        case TokenType::Bool:
            pointer = llvm.createAlloca(static_cast<std::string>(node->identifier), llvm.boolType());
            break;
        case TokenType::Char:
            pointer = llvm.createAlloca(static_cast<std::string>(node->identifier), llvm.i8Type());
            break;
    }

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
