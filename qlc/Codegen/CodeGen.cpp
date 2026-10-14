#include "CodeGen.hpp"
#include <iostream>
#include <string>
#include <string_view>

CodeGen::CodeGen() : llvm("RunQ-Lang_Test") {
}

llvm::Value* CodeGen::compileFunction(FuncNode* node) {
    symbolTable.clear(); // new function scope

    switch (node->returnType) {
        case TokenType::I32:
            currentFunction = llvm.createFunc(std::string(node->identifier), llvm.i32Type());
            break;
        case TokenType::I8:
            currentFunction = llvm.createFunc(std::string(node->identifier), llvm.i8Type());
            break;
        case TokenType::F64:
            currentFunction = llvm.createFunc(std::string(node->identifier), llvm.f64Type());
            break;
        case TokenType::Bool:
            currentFunction = llvm.createFunc(std::string(node->identifier), llvm.boolType());
            break;
        case TokenType::Char:
            currentFunction = llvm.createFunc(std::string(node->identifier), llvm.i8Type());
            break;
        default:
            // todo: error handling
            return nullptr;
    }
    llvm.createBlock("entry", currentFunction);

    // for (auto& arg : currentFunction->args()) {
    //     std::string name = std::string(arg.getName());
    //     symbolTable[name] = {false, &arg, arg.getType()};
    // }

    for (const auto& statement : node->body) {
        compileExpression(statement.get());
    }
    return currentFunction;
}

llvm::Value* CodeGen::compileExpression(Node* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::Func: {
            return compileFunction(static_cast<FuncNode*>(node));
        }
        case NodeType::If: {
            return compileIf(static_cast<IfNode*>(node));
        }
        case NodeType::Return: {
            return compileReturn(static_cast<ReturnNode*>(node));
        }
        case NodeType::IntLiteral: {
            auto* literal = static_cast<IntLiteralNode*>(node);
            switch (literal->type) {
                case TokenType::I32:
                    return llvm.createIntLiteral(literal->value, 32);
                case TokenType::I8:
                    return llvm.createIntLiteral(literal->value, 8);
                default:
                    std::cout << "Unknown var type in Codegen" << std::endl;
                    return nullptr;
            }
        }
        case NodeType::FloatLiteral: {
            auto* literal = static_cast<FloatLiteralNode*>(node);
            return llvm.createF64Literal(literal->value); // todo: handling of f32 and potentially f128 later same as in ints
        }
        case NodeType::BoolLiteral: {
            auto* literal = static_cast<BoolLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 1, false); // i1 has to actually be u1, otherwise its either 0 or -1
        }
        case NodeType::CharLiteral: {
            auto* literal = static_cast<CharLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 8);
        }
        case NodeType::MathOperator: {
            auto* mathOp = static_cast<MathOperatorNode*>(node);
            llvm::Value* left = compileExpression(mathOp->operand1.get());
            llvm::Value* right = compileExpression(mathOp->operand2.get());

            switch (mathOp->operatorType) {
                case TokenType::Plus:  return llvm.createPlus(left, right);
                case TokenType::Dash:  return llvm.createMinus(left, right);
                case TokenType::Star:  return llvm.createMultiply(left, right);
                case TokenType::Slash: return llvm.createDevide(left, right);
                default: return nullptr;
            }
        }
        case NodeType::Compare: {
            auto* cmp = static_cast<CompareNode*>(node);
            llvm::Value* operand1 = compileExpression(cmp->operand1.get());
            llvm::Value* operand2 = compileExpression(cmp->operand2.get());
            return llvm.createCompare(cmp->compareType , operand1, operand2);
        }
        case NodeType::VarDeclare: {
            return compileVarDeclare(static_cast<VarDeclareNode*>(node));
        }
        case NodeType::VarLoad: {
            return compileVarLoad(static_cast<VarLoadNode*>(node));
        }
        case NodeType::VarMod: {
            return compileVarMod(static_cast<VarModNode*>(node));
        }
        case NodeType::FuncLoad: {
            return compileFuncLoad(static_cast<FuncLoadNode*>(node));
        }
        case NodeType::While: {
            return compileWhile(static_cast<WhileNode*>(node));
        }
        default:
            std::cerr << "Unknown node type in CodeGen" << std::endl;
            return nullptr;
    }
    return nullptr;
}

llvm::Value* CodeGen::compileReturn(ReturnNode* node) { // todo: return statement typing
    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createReturn(val);
}

llvm::Type* CodeGen::llvmTypeFromToken(TokenType type) {
    switch (type) {
        case TokenType::I32:  return llvm.i32Type();
        case TokenType::I8:   return llvm.i8Type();
        case TokenType::F64:  return llvm.f64Type();
        case TokenType::Bool: return llvm.boolType();
        case TokenType::Char: return llvm.i8Type();
        default: return nullptr;
    }
}

llvm::Value* CodeGen::compileVarDeclare(VarDeclareNode* node) {
    if (!node->isMutable) {
        // immutable - SSA value, no alloca needed
        llvm::Value* val = compileExpression(node->body.get());
        llvm::Type* type = llvmTypeFromToken(node->valueType);
        symbolTable[std::string(node->identifier)] = {false, val, type};
        return val;
    }

    // mutable - alloca/store/load
    llvm::Value* pointer;
    llvm::Type* type;
    switch (node->valueType) {
        case TokenType::I32:
            pointer = llvm.createAlloca(std::string(node->identifier), llvm.i32Type());
            type = llvm.i32Type();
            break;
        case TokenType::I8:
            pointer = llvm.createAlloca(std::string(node->identifier), llvm.i8Type());
            type = llvm.i8Type();
            break;
        case TokenType::F64:
            pointer = llvm.createAlloca(std::string(node->identifier), llvm.f64Type());
            type = llvm.f64Type();
            break;
        case TokenType::Bool:
            pointer = llvm.createAlloca(std::string(node->identifier), llvm.boolType());
            type = llvm.boolType();
            break;
        case TokenType::Char:
            pointer = llvm.createAlloca(std::string(node->identifier), llvm.i8Type());
            type = llvm.i8Type();
            break;
        default:
            return nullptr;
    }

    symbolTable[std::string(node->identifier)] = {true, pointer, type};
    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createStore(val, pointer);
}

llvm::Value * CodeGen::compileVarLoad(VarLoadNode *node) {
    auto& entry = symbolTable[std::string(node->identifier)];
    if (entry.isMutable) {
        return llvm.createLoad(entry.type, entry.value); // value is pointer
    }
    return entry.value; // value is direct SSA value
}

llvm::Value * CodeGen::compileVarMod(VarModNode *node) {
    auto& entry = symbolTable[std::string(node->identifier)];
    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createStore(val, entry.value); // entry.value is the pointer
}

llvm::Value * CodeGen::compileFuncLoad(FuncLoadNode *node) {
    llvm::Function* fn = llvm.getFunction(std::string(node->identifier));
    if (!fn) {
        std::cerr << "Unknown function: " << node->identifier << std::endl;
        return nullptr;
    }

    std::vector<llvm::Value*> args;
    // for (const auto& arg : node->args) {x
    //     args.push_back(compileExpression(arg.get()));
    // }

    return llvm.createFuncCall(std::string(node->identifier), {});
}

llvm::Value* CodeGen::compileIf(IfNode* node) {
    llvm::Value* condition = compileExpression(node->condition.get());
    bool doesElseExist = !node->elseBody.empty();

    // create all blocks
    llvm::BasicBlock* ifBlock = llvm.createBlockNoInsert("if", currentFunction);
    llvm::BasicBlock* elseBlock = llvm.createBlockNoInsert("else", currentFunction);
    llvm::BasicBlock* mergeBlock = llvm.createBlockNoInsert("ifMerge", currentFunction);

    // branch
    llvm.createCondBranch(condition, ifBlock, elseBlock);

    // if block
    llvm.setInsertPoint(ifBlock);
    for (const auto& statement : node->ifBody) {
        compileExpression(statement.get());
    }
    if (!llvm.currentBlockHasTerminator()) {
        llvm.createBranch(mergeBlock);
    }

    // else block
    llvm.setInsertPoint(elseBlock);
    if (doesElseExist) {
        for (const auto& statement : node->elseBody) {
            compileExpression(statement.get());
        }
        if (!llvm.currentBlockHasTerminator()) {
            llvm.createBranch(mergeBlock);
        }
    }
    else {
        llvm.createBranch(mergeBlock);
    }

    llvm.setInsertPoint(mergeBlock);
    return nullptr;
}

llvm::Value * CodeGen::compileWhile(WhileNode *node) { // todo: return in while not handled
    llvm::BasicBlock* conditionBlock = llvm.createBlockNoInsert("whileCondition", currentFunction);
    llvm::BasicBlock* bodyBlock = llvm.createBlockNoInsert("whileBody", currentFunction);
    llvm::BasicBlock* mergeBlock = llvm.createBlockNoInsert("whileMerge", currentFunction);

    llvm.createBranch(conditionBlock);
    llvm.setInsertPoint(conditionBlock);

    llvm::Value* condition = compileExpression(node->condition.get());
    llvm.createCondBranch(condition, bodyBlock, mergeBlock);

    llvm.setInsertPoint(bodyBlock);
    for (const auto& statement : node->body) {
        compileExpression(statement.get());
    }
    llvm.createBranch(conditionBlock);
    llvm.setInsertPoint(mergeBlock);

    return nullptr;
}

void CodeGen::compile(Ast& ast, bool generateCompiledFiles) {
    if (!ast.root) { std::cout << "no AST found" << std::endl; return; }

    if (ast.root->type == NodeType::Root) {
        auto node = static_cast<RootNode*>(ast.root.get());
        for (const auto& statement : node->body) {
            compileExpression(statement.get());
        }
    }

    std::cout << "\n--- LLVM IR Output ---\n";
    llvm.print();

    if (!generateCompiledFiles) return;

    std::cout << "\n--- COMPILING ---\n";
    llvm.emitObjectFile("output.o");
    llvm.emitAssembly("output.s");
    system("clang output.o -o output");
    std::cout << "Executable: ./output" << std::endl;
}
