#include "CodeGen.hpp"
#include <iostream>

void CodeGen::compileFunction(FuncNode *node) {

    // defines function type (i32) todo: more function types
    llvm::Type* i32Type = llvm::Type::getInt32Ty(*context);
    llvm::FunctionType* funcType = llvm::FunctionType::get(i32Type, {}, false); // return i32, doesnt take arguments

    // creates the function
    llvm::Function* llvmFunc = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage, // External means the OS can see it to run it
        node->identifier,            // "main"
        module.get()                     // The blueprint it belongs to
    );

    // creates entry block
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(*context, "entry", llvmFunc);

    builder->SetInsertPoint(entryBlock); // moves to cursor into enteryblock.

    // loop through the body and pass every statement to our router
    for (const auto& statement : node->body) { // ?
        compileLiteral(statement.get());
    }
}

llvm::Value* CodeGen::compileLiteral(Node* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::RetInt:
            return compileReturn(static_cast<RetIntNode*>(node)); // ?

            // todo: add future features here

        default:
            std::cerr << "Unknown node type in CodeGen!" << std::endl;
            return nullptr;
    }
}

llvm::Value* CodeGen::compileReturn(RetIntNode* node) {
    llvm::Value* retVal = llvm::ConstantInt::get(*context, llvm::APInt(32, node->value, true));
    return builder->CreateRet(retVal);
}

CodeGen::CodeGen() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("RunQ-Lang_Module", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

void CodeGen::generate(Ast &ast) {
    if (!ast.root) return;

    if (ast.root->type == NodeType::Func) {
        compileFunction(static_cast<FuncNode*>(ast.root.get()));
    }

    std::cout << "\n--- LLVM IR Output ---\n";
    module->print(llvm::outs(), nullptr);
}