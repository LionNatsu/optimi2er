#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>

void codegen(llvm::LLVMContext &ctx, llvm::Module &module);

void optimize(llvm::Module &module);

llvm::TargetMachine *create_target_machine();

void print_asm(llvm::Module &module, llvm::TargetMachine *target_machine);

void generate_obj(llvm::Module &module, llvm::TargetMachine *target_machine, const char *filename);

int main() {
    llvm::LLVMContext ctx;
    llvm::Module module("module_1", ctx);

    auto target_machine = create_target_machine();
    if (!target_machine) {
        return 1;
    }

    module.setDataLayout(target_machine->createDataLayout());
    module.setTargetTriple(target_machine->getTargetTriple().str());

    std::cerr << "== LLVM IR Codegen ==" << std::endl;
    codegen(ctx, module);
    module.dump();

    std::cerr << "== LLVM IR Optimize ==" << std::endl;
    optimize(module);
    module.dump();

    std::cerr << "== Target Assembly ==" << std::endl;
    print_asm(module, target_machine);

    std::cerr << "== Target Object ==" << std::endl;
    generate_obj(module, target_machine, "hello.o");
    return 0;
}

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

llvm::TargetMachine *create_target_machine() {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string error;
    std::string target_triple = llvm::sys::getDefaultTargetTriple();
    auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if (!target) {
        std::cerr << error << std::endl;
        return nullptr;
    }

    return target->createTargetMachine(
            target_triple,
            "generic",
            "",
            llvm::TargetOptions{},
            llvm::Optional<llvm::Reloc::Model>{});

}

void target_output(llvm::Module &module, llvm::TargetMachine *target_machine, llvm::raw_pwrite_stream &s,
                   llvm::CodeGenFileType ft) {
    llvm::legacy::PassManager pass_manager;
    target_machine->addPassesToEmitFile(pass_manager, s, nullptr, ft);
    pass_manager.run(module);
}

void print_asm(llvm::Module &module, llvm::TargetMachine *target_machine) {
    target_output(module, target_machine, llvm::errs(), llvm::CGFT_AssemblyFile);
}

void generate_obj(llvm::Module &module, llvm::TargetMachine *target_machine, const char *filename) {
    std::error_code err;
    llvm::raw_fd_ostream dest(filename, err);
    target_output(module, target_machine, dest, llvm::CGFT_ObjectFile);
    std::cerr << " => " << filename << std::endl;
}
