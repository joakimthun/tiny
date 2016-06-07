#pragma once

#include <stack>

#include "type.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace tiny {
	
	struct AST;
	struct ASTNode;
	struct FnDeclaration;
	struct VarDeclaration;
	struct BinaryOperator;
	struct Identifier;
	struct IntLiteral;
	struct StringLiteral;
	struct RetDeclaration;
	struct CallExp;
	struct ArgDeclaration;

	struct CodegenResult
	{
		CodegenResult(llvm::Value* v) : value(v) {}
		llvm::Value* value;
	};

	class CodeGen
	{
	public:
		CodeGen(llvm::TargetMachine* tm);

		std::unique_ptr<CodegenResult> visit(AST* ast);
		std::unique_ptr<CodegenResult> visit(FnDeclaration* node);
		std::unique_ptr<CodegenResult> visit(ArgDeclaration* node);
		std::unique_ptr<CodegenResult> visit(VarDeclaration* node);
		std::unique_ptr<CodegenResult> visit(BinaryOperator* node);
		std::unique_ptr<CodegenResult> visit(Identifier* node);
		std::unique_ptr<CodegenResult> visit(IntLiteral* node);
		std::unique_ptr<CodegenResult> visit(StringLiteral* node);
		std::unique_ptr<CodegenResult> visit(RetDeclaration* node);
		std::unique_ptr<CodegenResult> visit(CallExp* node);

		std::unique_ptr<llvm::Module> execute(AST* ast);

	private:
		std::unique_ptr<CodegenResult> create_codegen_result(llvm::Value* v);

		static llvm::Type* get_llvm_type(const TinyType* type);

		std::unique_ptr<llvm::Module> module_;
		llvm::IRBuilder<> builder_;
	};

}