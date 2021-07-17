#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Passes/PassBuilder.h>

void codegen(llvm::LLVMContext &ctx, llvm::Module &module) {
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
}

void optimize(llvm::Module &module) {
    llvm::PassBuilder pass_builder;
    llvm::LoopAnalysisManager loop_analysis_manager;
    llvm::FunctionAnalysisManager function_analysis_manager;
    llvm::CGSCCAnalysisManager cgscc_analysis_manager;
    llvm::ModuleAnalysisManager module_analysis_manager;
    pass_builder.registerModuleAnalyses(module_analysis_manager);
    pass_builder.registerCGSCCAnalyses(cgscc_analysis_manager);
    pass_builder.registerFunctionAnalyses(function_analysis_manager);
    pass_builder.registerLoopAnalyses(loop_analysis_manager);
    pass_builder.crossRegisterProxies(
            loop_analysis_manager,
            function_analysis_manager,
            cgscc_analysis_manager,
            module_analysis_manager);

    llvm::ModulePassManager module_pass_manager =
            pass_builder.buildPerModuleDefaultPipeline(llvm::PassBuilder::OptimizationLevel::O3);
    module_pass_manager.run(module, module_analysis_manager);
}

int main() {
    llvm::LLVMContext ctx;
    llvm::Module module("jit", ctx);

    std::cout << "== Codegen ==" << std::endl;
    codegen(ctx, module);
    module.dump();

    std::cout << "== Optimize ==" << std::endl;
    optimize(module);
    module.dump();

    return 0;
}
