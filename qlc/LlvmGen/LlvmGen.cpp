#include "LlvmGen.hpp"
#include <iostream>
#include <ranges>
#include <string>

LlvmGen::LlvmGen() : llvm("RunQ-Lang_Test") {
}

void LlvmGen::declareFunction(FuncAstNode *node) {
    //std::cout << "entering switch" << std::endl;
    currentFunction = llvm.createFunc(std::string(node->identifier), llvm.createType(node->returnType), llvm.argsToLlvmTypes(node->args));
}

llvm::Value* LlvmGen::compileFunction(FuncAstNode* node) {
    //std::cout << "entering compilefunc" << std::endl;
    symbolTable.clear(); // new function scope

    //std::cout << "finishing switch" << std::endl;

    llvm::Function* fn = llvm.getFunction(std::string(node->identifier));
    currentFunction = fn;

    llvm.createBlock("entry", currentFunction);

    //std::cout << "entering adding args to table" << std::endl;

    size_t index = 0;
    for (auto& arg : currentFunction->args()) { // this goes where?
        if (index < node->args.size()) {
            std::string name(node->args[index].identifier);
            symbolTable[name] = {false, &arg, arg.getType()};
        }
        index++;
    }

    //std::cout << "entering body" << std::endl;

    for (const auto& statement : node->body) {
        compileExpression(statement.get());
    }

    if (node->returnType.kind == Void_ && !llvm.currentBlockHasTerminator()) { // creates return in void function if there isnt one
        llvm.createReturn(nullptr);
    }

    //std::cout << "finished compilefunc" << std::endl;
    return currentFunction;
}

llvm::Value * LlvmGen::compileExternFunction(ExternFuncAstNode *node) {
    symbolTable.clear(); // new function scope

    //std::cout << "entering switch" << std::endl;
    currentFunction = llvm.createFunc(std::string(node->identifier), llvm.createType(node->returnType), llvm.argsToLlvmTypes(node->args));

    size_t index = 0;
    for (auto& arg : currentFunction->args()) {
        if (index < node->args.size()) {
            std::string name(node->args[index].identifier);
            symbolTable[name] = {false, &arg, arg.getType()};
        }
        index++;
    }

    return currentFunction;
}

llvm::Value* LlvmGen::compileExpression(AstNode* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case AstNodeType::Func: {
            return compileFunction(static_cast<FuncAstNode*>(node));
        }
        // case NodeType::ExternFunc: {
        //     return compileExternFunction(static_cast<ExternFuncNode*>(node));
        // }
        case AstNodeType::If: {
            return compileIf(static_cast<IfNode*>(node));
        }
        case AstNodeType::Return: {
            return compileReturn(static_cast<ReturnAstNode*>(node));
        }
        case AstNodeType::IntLiteral: {
            auto* literal = static_cast<IntLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, literal->type.bitSize, literal->type.isSigned);
        }
        case AstNodeType::FloatLiteral: {
            auto* literal = static_cast<FloatLiteralNode*>(node);
            return llvm.createF64Literal(literal->value); // todo: handling of other sized floats
        }
        case AstNodeType::BoolLiteral: {
            auto* literal = static_cast<BoolLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 1, false); // i1 has to actually be u1, otherwise its either 0 or -1
        }
        case AstNodeType::CharLiteral: {
            auto* literal = static_cast<CharLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 8, true); // todo: char can be not signed
        }
        case AstNodeType::MathOperator: {
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
        case AstNodeType::Compare: {
            auto* cmp = static_cast<CompareNode*>(node);
            llvm::Value* operand1 = compileExpression(cmp->operand1.get());
            llvm::Value* operand2 = compileExpression(cmp->operand2.get());
            return llvm.createCompare(cmp->compareType , operand1, operand2);
        }
        case AstNodeType::VarDeclare: {
            return compileVarDeclare(static_cast<VarDeclareNode*>(node));
        }
        case AstNodeType::VarLoad: {
            return compileVarLoad(static_cast<VarLoadNode*>(node));
        }
        case AstNodeType::VarMod: {
            return compileVarMod(static_cast<VarModNode*>(node));
        }
        case AstNodeType::FuncLoad: {
            return compileFuncLoad(static_cast<FuncLoadNode*>(node));
        }
        case AstNodeType::While: {
            return compileWhile(static_cast<WhileNode*>(node));
        }
        default:
            //std::cerr << "Unknown node type in CodeGen" << std::endl;
            return nullptr;
    }
    return nullptr;
}

llvm::Value* LlvmGen::compileReturn(ReturnAstNode* node) { // todo: return statement typing
    if (!node->body) return llvm.createReturn(nullptr); // when return is void
    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createReturn(val);
}

llvm::Value* LlvmGen::compileVarDeclare(VarDeclareNode* node) {
    if (!node->isMutable) {
        // immutable - SSA value, no alloca needed
        llvm::Value* val = compileExpression(node->body.get());
        llvm::Type* type = llvm.createType(node->valueType);
        symbolTable[std::string(node->identifier)] = {false, val, type};
        return val;
    }

    // mutable - alloca/store/load
    llvm::Type* type = llvm.createType(node->valueType);
    llvm::Value* pointer = llvm.createAlloca(std::string(node->identifier), type);


    symbolTable[std::string(node->identifier)] = {true, pointer, type};
    llvm::Value* val = compileExpression(node->body.get());
    // if (node->valueType == TokenType::I8 && val->getType() == llvm.i32Type()) { obsolete??????
    //     val = llvm.createIntLiteral(static_cast<IntLiteralNode*>(node->body.get())->value, 8);
    // }
    return llvm.createStore(val, pointer);
}

llvm::Value * LlvmGen::compileVarLoad(VarLoadNode *node) {
    auto& entry = symbolTable[std::string(node->identifier)];
    if (entry.isMutable) {
        return llvm.createLoad(entry.type, entry.value); // value is pointer
    }
    return entry.value; // value is direct SSA value
}

llvm::Value * LlvmGen::compileVarMod(VarModNode *node) {
    auto& entry = symbolTable[std::string(node->identifier)];
    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createStore(val, entry.value); // entry.value is the pointer
}

llvm::Value * LlvmGen::compileFuncLoad(FuncLoadNode *node) {
    //std::cout << "entering compilefuncload" << std::endl;
    llvm::Function* fn = llvm.getFunction(std::string(node->identifier));
    if (!fn) {
        std::cerr << "Unknown function: " << node->identifier << std::endl;
        return nullptr;
    }

    //std::cout << "bfore args" << std::endl;
    std::vector<llvm::Value*> args;
    for (const auto& arg : node->args) {
        args.push_back(compileExpression(arg.get()));
    }
    //std::cout << "after args" << std::endl;
    return llvm.createFuncCall(std::string(node->identifier), args);
}

llvm::Value* LlvmGen::compileIf(IfNode* node) {
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

llvm::Value * LlvmGen::compileWhile(WhileNode *node) { // todo: return in while not handled
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

void LlvmGen::compile(Ast& ast, bool generateCompiledFiles, bool printIr, bool isLibrary, bool emitAsm, std::string outputLocation) {
    if (!ast.root) { std::cout << "no AST found" << std::endl; return; }

    if (ast.root->type == AstNodeType::Root) {
        auto node = static_cast<RootNode*>(ast.root.get());

        // first pass
        for (const auto& statement : node->body) {
            if (statement->type == AstNodeType::Func) {
                declareFunction(static_cast<FuncAstNode*>(statement.get()));
            }
            else if (statement->type == AstNodeType::ExternFunc) {
                compileExternFunction(static_cast<ExternFuncAstNode*>(statement.get()));
            }
        }

        // second pass
        for (const auto& statement : node->body) {
            compileExpression(statement.get());
        }
    }

    if (printIr) {
        std::cout << "\n--- LLVM IR Output ---\n";
        llvm.print();
    }

    if (!generateCompiledFiles) return;

    std::cout << "\n--- COMPILING ---\n"; // todo: support for non-bash system
    system("mkdir -p bin/obj");
    system("mkdir -p bin/asm");

    llvm.emitObjectFile("bin/obj/outputRunq.o");
    if (emitAsm) llvm.emitAssembly("bin/asm/outputRunq.s");

    std::string objStr;

    for (int i = 0; i < cList.size(); i++) { // gets object files
        std::string sourceFile = cList[i];
        std::string objFile = "bin/obj/" + std::to_string(i) + ".o"; // todo: find a better way to generate names

        std::string commandTemp = "clang -c " + sourceFile + " -o " + objFile;

        std::cout << "Running: " << commandTemp << std::endl;

        system(commandTemp.c_str());
        objStr += objFile + " ";

        //std::cout << cList[i] << std::endl;
        //std::cout << objFile << std::endl;
    }

    objStr += " bin/obj/outputRunq.o "; // gets RunQ files

    for (int i = 0; i < libList.size(); i++) { // gets archive files
        objStr += libList[i] + " "; // todo nesting archives will cause a crash: fix later
    }

    std::string command;

    if (isLibrary) {
        std::string archivePath;

        if (outputLocation.empty()) archivePath = "bin/lib.a";
        else archivePath = outputLocation + "/lib.a";

        command = "ar rcs " + archivePath + " " + objStr;
        std::cout << "Archiving: " << command << std::endl;
        system(command.c_str());
        std::cout << "Archive: " << archivePath << std::endl;
    }
    else {
        std::string outputPath;

        if (outputLocation.empty()) outputPath = "bin/output";
        else outputPath = outputLocation + "/output";

        command = "clang " + objStr + " -o " + outputPath;
        std::cout << "Linking: " << command << std::endl;
        system(command.c_str());
        std::cout << "Executable: ./" + outputPath << std::endl;
    }

    std::cout << "Done" << std::endl;
}
