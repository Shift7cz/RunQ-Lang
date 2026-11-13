#include "Ast.hpp"
#include "../Lexer/Lexer.hpp"

#include <iostream>

void Ast::print() { // helper print function, debugging.
    if (!root) {
        std::cout << "AST is empty.\n";
        return;
    }
    std::cout << "--- AST Structure ---\n";
    printNode(root.get(), 0);
}

void Ast::add(Ast &otherAst) {
    RootNode& otherRoot = static_cast<RootNode&>(*otherAst.root);
    RootNode* myRoot = static_cast<RootNode*>(root.get());
    for (int i = 0; i < otherRoot.body.size(); i++) {
        myRoot->body.push_back(std::move(otherRoot.body[i]));
    }
}

std::string Ast::typeToString(Type type) {
    std::string str;
    if (type.kind == Int_) {
        if (type.isSigned) {
            str = "i";
        }
        else {
            str = "u";
        }

        str += std::to_string(type.bitSize);
        return str;
    }
    if (type.kind == Float_) {
        str = "f";
        str += std::to_string(type.bitSize);
        return str;
    }
    switch (type.kind) {
        case Bool_: return "bool";
        case Char_: return "char";
        case Void_: return "void";
        default: return "ERROR in Ast";
    }
}

void Ast::printNode(AstNode* node, int indent) { // helper print function, {ai/2}
    if (!node) return;

    // Create indentation string (2 spaces per level)
    std::string spaces(indent * 2, ' ');

    switch (node->type) {
        case AstNodeType::Root: {
            std::cout << spaces << "[Root] \n";
            auto root = static_cast<RootNode*>(node);

            for (const auto& child : root->body) {
                printNode(child.get(), indent + 1);
            }
            break;
        }
        case AstNodeType::Func: {
            // 1. Cast to FuncNode to access unique features (.identifier and .body)
            auto func = static_cast<FuncAstNode*>(node);
            std::cout << spaces << "[[Function]] -> name: '" << func->identifier << "'; [Return Type] -> '" << typeToString(func->returnType) << "'\n";

            std::cout << spaces << "[[Function]][Args]" << std::endl;
            //indent += 1;
            for (const auto& child : func->args) {
                std::cout << spaces << "[Argument] -> name: '" << child.identifier << "'; Type ->'" << typeToString(child.type) << "'" << std::endl;
            }
            //indent -= 1;

            // 2. Recursively print all statements inside the function body
            std::cout << spaces << "[[Function]][Body]" << std::endl;
            for (const auto& child : func->body) {
                printNode(child.get(), indent + 1); // Increase indent for children!
            }
            break;
        }
        case AstNodeType::ExternFunc: {
                        // 1. Cast to FuncNode to access unique features (.identifier and .body)
            auto func = static_cast<ExternFuncAstNode*>(node);
            std::cout << spaces << "[[Extern Function]] -> name: '" << func->identifier << "'; [Return Type] -> '" << typeToString(func->returnType) << "'\n";

            std::cout << spaces << "[[Extrn Function]][Args]" << std::endl;
            //indent += 1;
            for (const auto& child : func->args) {
                std::cout << spaces << "[Argument] -> name: '" << child.identifier << "'; Type ->'" << typeToString(child.type) << "'" << std::endl;
            }
            //indent -= 1;
            break;
        }
        case AstNodeType::Return: {
            auto ret = static_cast<ReturnAstNode*>(node);
            std::cout << spaces << "[Return Statement] \n";

            printNode(ret->body.get(), indent + 1);
            break;
        }
        case AstNodeType::If: {
            auto ifNode = static_cast<IfNode*>(node);
            std::cout << spaces << "[[IF STATEMENT]] [Condition] \n";

            printNode(ifNode->condition.get(), indent + 1);

            std::cout << spaces << "[[IF STATEMENT]] [If Block] \n";
            for (const auto& child : ifNode->ifBody) {
                printNode(child.get(), indent + 1);
            }

            if (!ifNode->elseBody.empty()) {

                std::cout << spaces << "[[IF STATEMENT]] [Else Block] \n";
                for (const auto& child : ifNode->elseBody) {
                    printNode(child.get(), indent + 1);
                }
            }
            break;
        }
        case AstNodeType::While: {
            auto whileNode = static_cast<WhileNode*>(node);
            std::cout << spaces << "[[While]] [Condition] \n";

            printNode(whileNode->condition.get(), indent + 1);

            std::cout << spaces << "[[While]] [Block] \n";
            for (const auto& child : whileNode->body) {
                printNode(child.get(), indent + 1);
            }
            break;
        }
        case AstNodeType::VarDeclare: {
            auto var = static_cast<VarDeclareNode*>(node);
            std::cout << spaces << "[VarDeclare] -> name: '" << var->identifier << "'; [Type] -> '" << typeToString(var->valueType) << "'; [Mutable] -> '" << var -> isMutable << "'\n";

            printNode(var->body.get(), indent + 1);
            break;
        }
        case AstNodeType::VarLoad: {
            auto var = static_cast<VarLoadNode*>(node);
            std::cout << spaces << "[VarLoad] -> Name: '" << var->identifier << "'\n";
            break;
        }
        case AstNodeType::FuncLoad: {
            auto var = static_cast<FuncLoadNode*>(node);
            std::cout << spaces << "[FuncLoad] -> Name: '" << var->identifier << "' Args: \n";

            for (const auto& child : var->args) {
                printNode(child.get(), indent + 1);
            }

            break;
        }
        case AstNodeType::VarMod: {
            auto var = static_cast<VarModNode*>(node);
            std::cout << spaces << "[VarMod] -> Name: '" << var->identifier << "'\n";

            printNode(var->body.get(), indent + 1);
            break;
        }
        case AstNodeType::IntLiteral: {
            auto intl = static_cast<IntLiteralNode*>(node);
            std::cout << spaces << "[IntLiteral] -> value: '" << intl->value << "'; [Type] -> '" << typeToString(intl->type) << "'\n";
            break;
        }
        case AstNodeType::FloatLiteral: { // todo: multiple float types
            auto floatl = static_cast<FloatLiteralNode*>(node);
            std::cout << spaces << "[FloatLiteral] -> value: '" << floatl->value << "'\n";
            break;
        }
        case AstNodeType::BoolLiteral: {
            auto booll = static_cast<BoolLiteralNode*>(node);
            std::cout << spaces << "[BoolLiteral] -> value: '" << booll->value << "'\n";
            break;
        }
        case AstNodeType::CharLiteral: {
            auto charl = static_cast<CharLiteralNode*>(node);
            std::cout << spaces << "[CharLiteral] -> value: '" << charl->value << "'\n";
            break;
        }
        case AstNodeType::MathOperator: {
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
        case AstNodeType::Compare: {
            auto comp = static_cast<CompareNode*>(node);
            std::string_view compType;
            switch (comp->compareType) {
                case TokenType::IsEqualTo:
                    compType = "==";
                    break;
                case TokenType::NotEqualTo:
                    compType = "!=";
                    break;
                case TokenType::LessThan:
                    compType = "<";
                    break;
                case TokenType::GreaterThan:
                    compType = ">";
                    break;
                case TokenType::LessOrEqual:
                    compType = "<=";
                    break;
                case TokenType::GreaterOrEqual:
                    compType = ">=";
            }
            std::cout << spaces << "[Compare] -> Type: '" << compType << "'\n";

            printNode(comp->operand1.get(), indent + 1);
            printNode(comp->operand2.get(), indent + 1);

            break;
        }
    }
}
