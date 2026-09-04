#include "LlvmWrap.hpp"
#include <llvm/Support/raw_ostream.h>

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

void LlvmWrap::print() {
    module->print(llvm::outs(), nullptr);
}
