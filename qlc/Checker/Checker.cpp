#include "Checker.hpp"
#include "Logger.hpp"

void Checker::addFnEntry(FuncNode * node) {
    signatures[node->identifier] = FnEntry(node->args.size());
}

void Checker::addExternFnEntry(ExternFuncNode *node) {
    signatures[node->identifier] = FnEntry(node->args.size());
}

void Checker::validateCall(FuncLoadNode *node) {
    auto& entry = signatures[std::string(node->identifier)];
    if (entry.paramCount != node->args.size()) {
        Log::error("Error: Argument count mismatch in function call of " + node->identifier, 0, 0, "-"); // todo: nodes have line numbering or something
    }
}

void Checker::visit(AstNode* node) {
    if (!node) return;

    if (node->type == AstNodeType::FuncLoad) {
        validateCall(static_cast<FuncLoadNode*>(node));
        // still need to recurse into its args below
    }

    switch (node->type) {
        case AstNodeType::Func: {
            auto f = static_cast<FuncNode*>(node);
            for (auto& stmt : f->body) visit(stmt.get());
            break;
        }
        case AstNodeType::Return: {
            auto r = static_cast<ReturnNode*>(node);
            visit(r->body.get());
            break;
        }
        case AstNodeType::If: {
            auto i = static_cast<IfNode*>(node);
            visit(i->condition.get());
            for (auto& s : i->ifBody) visit(s.get());
            for (auto& s : i->elseBody) visit(s.get());
            break;
        }
        case AstNodeType::While: {
            auto i = static_cast<WhileNode*>(node);
            visit(i->condition.get());
            for (auto& s : i->body) visit(s.get());
            break;
        }
        case AstNodeType::VarDeclare: {
            auto i = static_cast<VarDeclareNode*>(node);
            visit(i->body.get());
            break;
        }
        case AstNodeType::FuncLoad: {
            auto fl = static_cast<FuncLoadNode*>(node);
            for (auto& arg : fl->args) visit(arg.get());
            break;
        }
        case AstNodeType::VarMod: {
            auto i = static_cast<VarModNode*>(node);
            visit(i->body.get());
            break;
        }
        case AstNodeType::MathOperator: {
            auto i = static_cast<MathOperatorNode*>(node);
            visit(i->operand1.get());
            visit(i->operand2.get());
            break;
        }
        case AstNodeType::Compare: {
            auto i = static_cast<CompareNode*>(node);
            visit(i->operand1.get());
            visit(i->operand2.get());
            break;
        }
        default: break; // leaf nodes: IntLiteral, VarLoad, etc. — nothing to recurse into
    }
}

void Checker::check(Ast& ast) {
    if (!ast.root) { Log::error("no AST found", 0, 0, ""); return; } // todo: source file

    if (ast.root->type == AstNodeType::Root) {
        auto node = static_cast<RootNode*>(ast.root.get());

        for (const auto& statement : node->body) { // pass one
            if (statement->type == AstNodeType::Func) addFnEntry(static_cast<FuncNode*>(statement.get()));
            else if (statement->type == AstNodeType::ExternFunc) addExternFnEntry(static_cast<ExternFuncNode*>(statement.get()));
        }

        for (const auto& statement : node->body) { // pass one
            visit(statement.get());
        }
    }
}

