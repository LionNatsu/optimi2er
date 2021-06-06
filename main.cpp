#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>

llvm::Value *codegen(llvm::LLVMContext &ctx) {
    auto t_int64 = llvm::IntegerType::get(ctx, 64);
    return llvm::ConstantInt::get(t_int64, 123);
}

int main() {
    llvm::LLVMContext ctx;
    std::cout << "== Codegen ==" << std::endl;
    auto code = codegen(ctx);
    code->dump();
    return 0;
}
