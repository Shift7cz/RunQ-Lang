#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>
#include <memory>
#include <string>

#include "Token.hpp"

class LlvmWrap {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

public:
    LlvmWrap(const std::string& moduleName);

    // types
    llvm::Type* i32Type();
    llvm::Type* i8Type();
    llvm::Type* f64Type(); // double
    llvm::Type* boolType();
    // char is i8

    // structure
    llvm::Function* createFunc(const std::string& name, llvm::Type* retType);
    llvm::BasicBlock* createBlock(const std::string& name, llvm::Function* fn);
    llvm::BasicBlock *createBlockNoInsert(const std::string &name, llvm::Function *fn);
    void setInsertPoint(llvm::BasicBlock *block);

    // instructions
    llvm::Value* createReturn(llvm::Value* value);
    llvm::Value* createIntLiteral(int value, int numBits, bool isSigned = true);
    llvm::Value *createF64Literal(double value);

    // math operations
    llvm::Value* createPlus(llvm::Value* operand1, llvm::Value* operand2);
    llvm::Value* createMinus(llvm::Value* operand1, llvm::Value* operand2);
    llvm::Value* createMultiply(llvm::Value* operand1, llvm::Value* operand2);
    llvm::Value* createDevide(llvm::Value* operand1, llvm::Value* operand2);

    // variable operations
    llvm::Value *createAlloca(const std::string &name, llvm::Type *type);
    llvm::Value *createStore(llvm::Value *value, llvm::Value *pointer);
    llvm::Value *createLoad(llvm::Type *type, llvm::Value *pointer);

    // control flow
    llvm::Value *createCondBranch(llvm::Value *condition, llvm::BasicBlock *thenBlock, llvm::BasicBlock *elseBlock);
    llvm::Value *createBranch(llvm::BasicBlock *block);

    llvm::Value *createCompare(TokenType op, llvm::Value *operand1, llvm::Value *operand2);

    // output
    void print();

    // compiling
    void emitObjectFile(const std::string& filename);
    void emitAssembly(const std::string& filename);
};

/*
 * Use:
 * todo: Documentation
 *
 * {ai}
 */