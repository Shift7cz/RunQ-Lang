#include "CodeGen.hpp"
#include <iostream>
#include <ranges>
#include <string>

CodeGen::CodeGen() : llvm("RunQ-Lang_Test") {
}

void CodeGen::declareFunction(FuncNode *node) {
    //std::cout << "entering switch" << std::endl;
    currentFunction = llvm.createFunc(std::string(node->identifier), llvm.createType(node->returnType), llvm.argsToLlvmTypes(node->args));
}

llvm::Value* CodeGen::compileFunction(FuncNode* node) {
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

llvm::Value * CodeGen::compileExternFunction(ExternFuncNode *node) {
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

llvm::Value* CodeGen::compileExpression(Node* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::Func: {
            return compileFunction(static_cast<FuncNode*>(node));
        }
        case NodeType::ExternFunc: {
            return compileExternFunction(static_cast<ExternFuncNode*>(node));
        }
        case NodeType::If: {
            return compileIf(static_cast<IfNode*>(node));
        }
        case NodeType::Return: {
            return compileReturn(static_cast<ReturnNode*>(node));
        }
        case NodeType::IntLiteral: {
            auto* literal = static_cast<IntLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, literal->type.bitSize, literal->type.isSigned);
        }
        case NodeType::FloatLiteral: {
            auto* literal = static_cast<FloatLiteralNode*>(node);
            return llvm.createF64Literal(literal->value); // todo: handling of other sized floats
        }
        case NodeType::BoolLiteral: {
            auto* literal = static_cast<BoolLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 1, false); // i1 has to actually be u1, otherwise its either 0 or -1
        }
        case NodeType::CharLiteral: {
            auto* literal = static_cast<CharLiteralNode*>(node);
            return llvm.createIntLiteral(literal->value, 8, true); // todo: char can be not signed
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
            //std::cerr << "Unknown node type in CodeGen" << std::endl;
            return nullptr;
    }
    return nullptr;
}

llvm::Value* CodeGen::compileReturn(ReturnNode* node) { // todo: return statement typing
    if (!node->body) return llvm.createReturn(nullptr); // when return is void
    llvm::Value* val = compileExpression(node->body.get());
    return llvm.createReturn(val);
}

llvm::Value* CodeGen::compileVarDeclare(VarDeclareNode* node) {
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

void CodeGen::compile(Ast& ast, bool generateCompiledFiles, bool printIr, bool isLibrary, bool emitAsm, std::string outputLocation) {
    if (!ast.root) { std::cout << "no AST found" << std::endl; return; }

    if (ast.root->type == NodeType::Root) {
        auto node = static_cast<RootNode*>(ast.root.get());

        // first pass
        for (const auto& statement : node->body) {
            declareFunction(static_cast<FuncNode*>(statement.get()));
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
