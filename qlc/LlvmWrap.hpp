#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>
#include <string>

class LlvmWrap {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

public:
    LlvmWrap(const std::string& moduleName);

    // types
    llvm::Type* i32Type();

    // structure
    llvm::Function* createFunc(const std::string& name, llvm::Type* retType);
    llvm::BasicBlock* createBlock(const std::string& name, llvm::Function* fn);

    // instructions
    llvm::Value* createReturn(llvm::Value* value);
    llvm::Value* createI32Literal(int value);

    // output
    void print();
};

/*
 * Use:
 * todo: Documentation
 *
 * {ai/2}
 */