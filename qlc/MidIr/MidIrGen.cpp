#include "MidIrGen.hpp"
#include "../Checker/Logger.hpp"

void MidIrGen::flattenAst(Ast ast) {
    if (!ast.root) { Log::error("no AST found in MidIr", 0, 0, ""); return; }
    if (ast.root->type != AstNodeType::Root) { Log::error("Ast root isnt root in MidIr", 0, 0, ""); return; }

    auto node = static_cast<RootNode*>(ast.root.get());

    for (int i = 0; i < node->body.size(); i++) {
        switch (node->body[i]->type) {
            case AstNodeType::Func:
                break;
            case AstNodeType::ExternFunc:
                break;
            // todo: structs, objects, enums, go here
            default:
                Log::error("Unknown root type in ast", 0, 0, ""); // todo: put error handling for this into parser
                return;
        }
    }
}