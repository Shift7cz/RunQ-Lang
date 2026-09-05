#include "LlvmWrap.hpp"

#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>

LlvmWrap::LlvmWrap(const std::string& moduleName) { // initilises llvm
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(moduleName, *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

llvm::Type* LlvmWrap::i32Type() {
    return llvm::Type::getInt32Ty(*context);
}

llvm::Function* LlvmWrap::createFunc(const std::string& name, llvm::Type* retType) {
    llvm::FunctionType* fnType = llvm::FunctionType::get(retType, {}, false);
    return llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, name, module.get());
}

llvm::BasicBlock* LlvmWrap::createBlock(const std::string& name, llvm::Function* fn) {
    llvm::BasicBlock* block = llvm::BasicBlock::Create(*context, name, fn);
    builder->SetInsertPoint(block);
    return block;
}

llvm::Value* LlvmWrap::createReturn(llvm::Value* value) {
    return builder->CreateRet(value);
}

llvm::Value* LlvmWrap::createI32Literal(int value) {
    return llvm::ConstantInt::get(*context, llvm::APInt(32, value, true));
}

llvm::Value * LlvmWrap::createPlus(llvm::Value *operand1, llvm::Value *operand2) {
    return builder->CreateAdd(operand1, operand2, "addtmp"); // Names are only for readability for now todo: review later
}

llvm::Value * LlvmWrap::createMinus(llvm::Value *operand1, llvm::Value *operand2) {
    return builder->CreateSub(operand1, operand2, "subtmp");
}

llvm::Value * LlvmWrap::createMultiply(llvm::Value *operand1, llvm::Value *operand2) {
    return builder->CreateMul(operand1, operand2, "multmp");
}

llvm::Value * LlvmWrap::createDevide(llvm::Value *operand1, llvm::Value *operand2) {
    return builder->CreateSDiv(operand1, operand2, "sdivtmp");  // S = signed, matches i32
}

llvm::Value* LlvmWrap::createAlloca(const std::string& name, llvm::Type* type) {
    return builder->CreateAlloca(type, nullptr, name);
}

llvm::Value* LlvmWrap::createStore(llvm::Value* value, llvm::Value* pointer) {
    return builder->CreateStore(value, pointer);
}

llvm::Value* LlvmWrap::createLoad(llvm::Type* type, llvm::Value* pointer) {
    return builder->CreateLoad(type, pointer, "loadtmp");
}

void LlvmWrap::print() {
    module->print(llvm::outs(), nullptr);
}

void LlvmWrap::emitObjectFile(const std::string& filename) { // {ai}
    // initialize LLVM's native target (your current CPU)
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    // get the target triple for your current platform
    // e.g. "x86_64-pc-linux-gnu" or "arm64-apple-macosx"
    std::string targetTripleStr = llvm::sys::getDefaultTargetTriple();
    llvm::Triple targetTriple(targetTripleStr);
    module->setTargetTriple(targetTriple);

    // look up the target in LLVM's registry
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        std::cerr << "Target error: " << error << std::endl;
        return;
    }

    // create the target machine (CPU, features, optimization level)
    llvm::TargetOptions options;
    llvm::TargetMachine* targetMachine = target->createTargetMachine(
        targetTriple,
        "generic", // CPU type, generic = current machine
        "",        // CPU features, empty = default
        options,
        llvm::Reloc::PIC_ // position independent code
    );

    // tell the module about the data layout (pointer sizes, alignment etc.)
    module->setDataLayout(targetMachine->createDataLayout());

    // open output file
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    if (ec) {
        std::cerr << "Could not open file: " << ec.message() << std::endl;
        return;
    }

    // emit object file
    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr,
        llvm::CodeGenFileType::ObjectFile)) {
        std::cerr << "Target cannot emit object file" << std::endl;
        return;
        }

    pass.run(*module);
    dest.flush();

    std::cout << "Object file written to: " << filename << std::endl;
}

void LlvmWrap::emitAssembly(const std::string& filename) {
    // initialize LLVM's native target (your current CPU)
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    // get the target triple for your current platform
    // e.g. "x86_64-pc-linux-gnu" or "arm64-apple-macosx"
    std::string targetTripleStr = llvm::sys::getDefaultTargetTriple();
    llvm::Triple targetTriple(targetTripleStr);
    module->setTargetTriple(targetTriple);

    // look up the target in LLVM's registry
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        std::cerr << "Target error: " << error << std::endl;
        return;
    }

    // create the target machine (CPU, features, optimization level)
    llvm::TargetOptions options;
    llvm::TargetMachine* targetMachine = target->createTargetMachine(
        targetTriple,
        "generic", // CPU type, generic = current machine
        "",        // CPU features, empty = default
        options,
        llvm::Reloc::PIC_ // position independent code
    );

    // tell the module about the data layout (pointer sizes, alignment etc.)
    module->setDataLayout(targetMachine->createDataLayout());

    // open output file
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    if (ec) {
        std::cerr << "Could not open file: " << ec.message() << std::endl;
        return;
    }

    // Literary the same think as with object file but does assembly instead
    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr,
        llvm::CodeGenFileType::AssemblyFile)) {
        std::cerr << "Target cannot emit assembly file" << std::endl;
        return;
        }

    pass.run(*module);
    dest.flush();

    std::cout << "Assembly file written to: " << filename << std::endl;
}