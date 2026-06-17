#pragma once

#include "Ast.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>

class CodeGen {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    void compileFunction(FuncNode* node); // todo: split fn and endpoint function
    llvm::Value *compileLiteral(Node *node);
    llvm::Value *compileReturn(RetIntNode *node);

public:
    CodeGen();

    void generate(Ast& ast);
};