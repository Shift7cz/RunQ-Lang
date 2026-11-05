#pragma once

#include <map>
#include "../Parser/Ast.hpp"

class Checker {
public:
    struct FnEntry {
        int paramCount;
    };

    std::map<std::string, FnEntry> signatures;

    void addFnEntry(FuncNode * node);
    void addExternFnEntry(ExternFuncNode * node);

    void validateCall(FuncLoadNode * node);
    void visit(Node * node);

    void check(Ast& ast);
};