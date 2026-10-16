#pragma once

#include <map>

#include "../Parser/Ast.hpp"
#include "../Project/Project.hpp"
#include "LlvmWrap.hpp"

class CodeGen {
private:
    struct VarEntry {
        bool isMutable;
        llvm::Value* value;
        llvm::Type* type;
    };

    std::map<std::string, VarEntry> symbolTable;

    LlvmWrap llvm;
    llvm::Function* currentFunction = nullptr;

    llvm::Value* compileFunction(FuncNode* node);
    llvm::Value* compileExternFunction(ExternFuncNode* node);
    llvm::Value* compileExpression(Node* node);
    llvm::Value* compileReturn(ReturnNode* node);
    llvm::Type* llvmTypeFromToken(TokenType type);
    llvm::Value* compileVarDeclare(VarDeclareNode* node);
    llvm::Value* compileVarLoad(VarLoadNode* node);
    llvm::Value* compileVarMod(VarModNode* node);
    llvm::Value* compileFuncLoad(FuncLoadNode* node);
    llvm::Value* compileIf(IfNode* node);
    llvm::Value* compileWhile(WhileNode* node);

public:
    std::vector<std::string> cList;

    CodeGen();
    void compile(Ast& ast, bool generateCompiledFiles, bool printIr);
};