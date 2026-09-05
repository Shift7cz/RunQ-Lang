#pragma once

#include <map>

#include "Ast.hpp"
#include "LlvmWrap.hpp"

class CodeGen {
private:
    LlvmWrap llvm;
    std::map<std::string, llvm::Value*> symbolTable;

    void compileFunction(FuncNode* node);
    llvm::Value* compileExpression(Node* node);
    llvm::Value* compileReturn(ReturnNode* node);
    llvm::Value* compileVarDeclare(VarDeclareNode* node);
    llvm::Value* compileVarLoad(VarLoadNode* node);

public:
    CodeGen();
    void compile(Ast& ast);
};