#pragma once

#include "Ast.hpp"
#include "LlvmWrap.hpp"

class CodeGen {
private:
    LlvmWrap llvm;

    void compileFunction(FuncNode* node);
    llvm::Value* compileExpression(Node* node);
    llvm::Value* compileReturn(ReturnNode* node);

public:
    CodeGen();
    void generate(Ast& ast);
};