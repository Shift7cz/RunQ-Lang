#include "CodeGen.hpp"
#include <iostream>

CodeGen::CodeGen() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("RunQ-Lang_Module", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

void CodeGen::generate(Ast &ast) {
    if (!ast.root) return;

    auto* funcNode = static_cast<FuncNode*>(ast.root.get()); // grabs the root node.

    // defines function type (i32)
    llvm::Type* i32Type = llvm::Type::getInt32Ty(*context);
    llvm::FunctionType* funcType = llvm::FunctionType::get(i32Type, {}, false); // return i32, doesnt take arguments

    // creates the function
    llvm::Function* llvmFunc = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage, // External means the OS can see it to run it
        funcNode->identifier,            // "main"
        module.get()                     // The blueprint it belongs to
    );

    // creates entry block
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(*context, "entry", llvmFunc);

    builder->SetInsertPoint(entryBlock); // moves to cursor into enteryblock.

    // translates the body
    for (const auto& statement : funcNode->body) {

        if (statement->type == NodeType::RetInt) { // its a return statement
            auto* retNode = static_cast<RetIntNode*>(statement.get());

            // converts the value into an LLVM Arbitrary Precision Integer (APInt).
            // 32 = bit width, retNode->value = the number 67, true = it is signed.
            llvm::Value* retVal = llvm::ConstantInt::get(*context, llvm::APInt(32, retNode->value, true));

            // writes ret i32 67
            builder->CreateRet(retVal);
        }
    }

    std::cout << "\n--- LLVM IR Output ---\n";
    module->print(llvm::outs(), nullptr);
}
