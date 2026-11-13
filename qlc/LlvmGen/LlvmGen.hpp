#pragma once

#include <map>

#include "../Parser/Ast.hpp"
#include "../Project/Project.hpp"
#include "LlvmWrap.hpp"

class LlvmGen {
private:
    struct VarEntry {
        bool isMutable;
        llvm::Value* value;
        llvm::Type* type;
    };

    std::map<std::string, VarEntry> symbolTable;

    LlvmWrap llvm;
    llvm::Function* currentFunction = nullptr;

    void declareFunction(FuncAstNode* node);
    llvm::Value* compileFunction(FuncAstNode* node);
    llvm::Value* compileExternFunction(ExternFuncAstNode* node);
    llvm::Value* compileExpression(AstNode* node);
    llvm::Value* compileReturn(ReturnAstNode* node);
    llvm::Value* compileVarDeclare(VarDeclareNode* node);
    llvm::Value* compileVarLoad(VarLoadNode* node);
    llvm::Value* compileVarMod(VarModNode* node);
    llvm::Value* compileFuncLoad(FuncLoadNode* node);
    llvm::Value* compileIf(IfNode* node);
    llvm::Value* compileWhile(WhileNode* node);

public:
    std::vector<std::string> cList;
    std::vector<std::string> libList;

    LlvmGen();
    void compile(Ast& ast, bool generateCompiledFiles, bool printIr, bool isLibrary, bool emitAsm, std::string outputLocation);
};