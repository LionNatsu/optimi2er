#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

llvm::Value *codegen(llvm::LLVMContext &ctx) {
    llvm::IRBuilder<> builder(ctx);
    auto v1 = llvm::ConstantInt::get(llvm::IntegerType::get(ctx, 64), 42);
    auto v2 = llvm::ConstantInt::get(llvm::IntegerType::get(ctx, 64), 100);
    auto v3 = llvm::ConstantInt::get(llvm::IntegerType::get(ctx, 64), 13);
    auto v4 = builder.CreateMul(v1, v2);
    return builder.CreateAdd(v4, v3);
}

int main() {
    llvm::LLVMContext ctx;
    std::cout << "== Codegen ==" << std::endl;
    auto code = codegen(ctx);
    code->dump();
    return 0;
}
