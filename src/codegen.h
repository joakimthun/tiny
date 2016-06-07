#pragma once

#include <stack>

#include "type.h"
#include "symbols.h"

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

	struct LLVMSymbol
	{
		LLVMSymbol(llvm::AllocaInst* v, Type t) : value(v), type(std::make_unique<TinyType>(t)) {}
		LLVMSymbol(llvm::AllocaInst* v, std::unique_ptr<TinyType> t) : value(v), type(std::move(t)) {}
		llvm::AllocaInst * value;
		std::unique_ptr<TinyType> type;
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
		static llvm::AllocaInst* create_alloca(llvm::Function* function, const std::string& name, llvm::Type* type);
		llvm::Function* get_function(const std::string& name) const;

		void push_scope(std::unique_ptr<SymbolTable<LLVMSymbol>> scope);
		void pop_scope();
		SymbolTable<LLVMSymbol>* current_scope();

		std::unique_ptr<CodegenResult> create_codegen_result(llvm::Value* v) const;

		static llvm::Type* get_llvm_type(const TinyType* type);
		static std::unique_ptr<TinyType> get_type(const llvm::Type* type);

		std::unique_ptr<llvm::Module> module_;
		llvm::IRBuilder<> builder_;
		std::stack<std::unique_ptr<SymbolTable<LLVMSymbol>>> scopes_;
	};

}