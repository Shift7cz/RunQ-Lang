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

    // math operations
    llvm::Value* createPlus(llvm::Value* operand1, llvm::Value* operand2);
    llvm::Value* createMinus(llvm::Value* operand1, llvm::Value* operand2);
    llvm::Value* createMultiply(llvm::Value* operand1, llvm::Value* operand2);
    llvm::Value* createDevide(llvm::Value* operand1, llvm::Value* operand2);

    // variable operations
    llvm::Value *createAlloca(const std::string &name, llvm::Type *type);
    llvm::Value *createStore(llvm::Value *value, llvm::Value *pointer);
    llvm::Value *createLoad(llvm::Type *type, llvm::Value *pointer);

    // output
    void print();
};

/*
 * Use:
 * todo: Documentation
 *
 * {ai}
 */