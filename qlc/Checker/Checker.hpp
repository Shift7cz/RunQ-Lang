#pragma once

#include <map>
#include "../Parser/Ast.hpp"

class Checker {
public:
    struct FnEntry {
        int paramCount;
    };

    std::map<std::string, FnEntry> signatures;

    void addFnEntry(FuncAstNode * node);
    void addExternFnEntry(ExternFuncAstNode * node);

    void validateCall(FuncLoadNode * node);
    void visit(AstNode * node);

    void check(Ast& ast);
};