#pragma once

#include <map>

#include "Ast.hpp"
#include "LlvmWrap.hpp"

class CodeGen {
private:
    LlvmWrap llvm;
    std::map<std::string, llvm::Value*> symbolTable;
    llvm::Function* currentFunction = nullptr;

    void compileFunction(FuncNode* node);
    llvm::Value* compileExpression(Node* node);
    llvm::Value* compileReturn(ReturnNode* node);
    llvm::Value* compileVarDeclare(VarDeclareNode* node);
    llvm::Value* compileVarLoad(VarLoadNode* node);
    llvm::Value* compileIf(IfNode* node);

public:
    CodeGen();
    void compile(Ast& ast, bool generateCompiledFiles);
};