#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>

llvm::Value *codegen(llvm::LLVMContext &ctx, llvm::Module &module) {
    llvm::FunctionType *fn_type = llvm::FunctionType::get(
            llvm::Type::getInt64Ty(ctx),
            {llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx)},
            false);
    llvm::Function *fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, "my_function", module);

    llvm::BasicBlock *block = llvm::BasicBlock::Create(ctx, "entry", fn);

    llvm::IRBuilder<> builder(ctx);
    builder.SetInsertPoint(block);
    auto v1 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 42);
    auto v2 = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 100);
    auto v3 = builder.CreateMul(v1, v2);
    auto v4 = fn->getArg(0);
    auto v5 = builder.CreateAdd(v4, v4);
    auto v6 = builder.CreateAdd(v5, v5);
    auto v7 = fn->getArg(1);
    auto v8 = builder.CreateMul(v6, v7);
    auto ret = builder.CreateMul(v3, v8);
    builder.CreateRet(ret);

    llvm::verifyFunction(*fn);
    return fn;
}

int main() {
    llvm::LLVMContext ctx;
    llvm::Module module("jit", ctx);
    std::cout << "== Codegen ==" << std::endl;
    [[maybe_unused]] auto code = codegen(ctx, module);
    module.dump();
    return 0;
}
