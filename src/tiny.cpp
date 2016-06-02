#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include "llvm/Support/ManagedStatic.h"
#include <llvm/IR/Verifier.h>

#include "type.h"
#include "tiny_exception.h"
#include "parser.h"
#include "codegen.h"
#include "jit.h"

using namespace tiny;

int main(int argc, char* argv[])
{
	try
	{
		llvm::InitializeNativeTarget();
		llvm::InitializeNativeTargetAsmPrinter();
		llvm::InitializeNativeTargetAsmParser();
		
		auto tm = llvm::EngineBuilder().selectTarget();

		auto p = std::make_unique<Parser>(std::make_unique<Lexer>("test_files/test.tiny"));

		auto ast = p->parse();

		auto codegen = std::make_unique<CodeGen>(tm);
		auto module = codegen->execute(ast.get());
		
		llvm::verifyModule(*module);

		module->dump();

		auto jit = std::make_unique<OrcJit>(*tm);
		jit->add_module(std::move(module));

		auto main_ptr = jit->get_function_ptr<i32()>("main");
		
		llvm::outs() << "\n";
		llvm::outs() << "main returns: ";
		llvm::outs() << main_ptr() << "\n";
		
		llvm::outs().flush();
		
		getchar();
		
		llvm::llvm_shutdown();

		return 0;
	}
	catch(TinyException e)
	{
		llvm::outs() << e.what() << "\n";

		return 1;
	}
}