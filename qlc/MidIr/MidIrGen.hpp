#pragma once

#include <map>
#include "Asl.hpp"
#include "../Parser/Ast.hpp"

class MidIrGen {
    enum RegisterEntryObj {
        Fn,
        ExternFn,
        ImutVar,
        MutVar
    };
    struct RegisterEntry {
        RegisterEntryObj obj;
        std::string name;
    };

    std::map<int, RegisterEntry> registers; // maps signatures to names to info about what it is

public:
    Asl asl;

    void flattenAst(Ast ast);
};