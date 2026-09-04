# include "Ast.hpp"

#include <iostream>

void Ast::print() { // helper print function, debugging.
    if (!root) {
        std::cout << "AST is empty.\n";
        return;
    }
    std::cout << "--- AST Structure ---\n";
    printNode(root.get(), 0);
}

void Ast::printNode(Node* node, int indent) { // helper print function, {ai/2}
    if (!node) return;

    // Create indentation string (2 spaces per level)
    std::string spaces(indent * 2, ' ');

    switch (node->type) {
        case NodeType::Func: {
            // 1. Cast to FuncNode to access unique features (.identifier and .body)
            auto func = static_cast<FuncNode*>(node);
            switch (func->returnType) {
                case TokenType::I32:
                    std::cout << spaces << "[Function] -> name: '" << func->identifier << "'; [Return Type] -> 'i32'\n";
                    break;
                    // todo: Other return types printing here
                default:
                    std::cout << spaces << "[Function] -> name: '" << func->identifier << "'; [Return Type] -> 'ERROR in Ast'\n";
                    break;
            }

            // 2. Recursively print all statements inside the function body
            for (const auto& child : func->body) {
                printNode(child.get(), indent + 1); // Increase indent for children!
            }
            break;
        }
        case NodeType::Return: {
            auto ret = static_cast<ReturnNode*>(node);
            std::cout << spaces << "[Return Statement] \n";

            printNode(ret->body.get(), indent + 1);
            break;
        }
        case NodeType::VarDeclare: {
            auto var = static_cast<VarDeclareNode*>(node);
            switch (var->valueType) {
                case TokenType::I32:
                    std::cout << spaces << "[VarDeclare] -> name: '" << var->identifier << "'; [Type] -> 'i32'\n";
                    break;
                    // todo: Other return types printing here
                default:
                    std::cout << spaces << "[VarDeclare] -> name: '" << var->identifier << "'; [Type] -> 'ERROR in Ast'\n";
                    break;
            }

            printNode(var->body.get(), indent + 1);
            break;
        }
        case NodeType::VarLoad: {
            auto var = static_cast<VarLoadNode*>(node);
            std::cout << spaces << "[VarLoad] -> Name: '" << var->identifier << "'\n";
            break;
        }
        case NodeType::I32Literal: {
            auto intl = static_cast<I32LiteralNode*>(node);
            std::cout << spaces << "[IntLiteral] -> value: '" << intl->value << "'\n";
            break;
        }
        case NodeType::MathOperator: {
            auto op = static_cast<MathOperatorNode*>(node);
            std::string_view optype;
            switch (op->operatorType) {
                case TokenType::Plus:
                    optype = "+";
                    break;
                case TokenType::Dash:
                    optype = "-";
                    break;
                case TokenType::Star:
                    optype = "*";
                    break;
                case TokenType::Slash:
                    optype = "/";
                    break;
            }
            std::cout << spaces << "[MathOperator] -> Operation: '" << optype << "'\n";

            printNode(op->operand1.get(), indent + 1);
            printNode(op->operand2.get(), indent + 1);

            break;
        }
    }
}
