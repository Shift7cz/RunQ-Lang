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
        case NodeType::Return:
            return compileReturn(static_cast<ReturnNode*>(node));

            // todo: I32Literal gets handled inside compileReturn for now will need its own case once expressions can appear standalone

        default:
            std::cerr << "Unknown node type in CodeGen!" << std::endl;
            return nullptr;
    }
}

llvm::Value* CodeGen::compileReturn(ReturnNode* node) {
    auto* literal = static_cast<I32LiteralNode*>(node->body.get()); // todo: only handles I32Literal for now
    llvm::Value* val = llvm.createI32Literal(literal->value);
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