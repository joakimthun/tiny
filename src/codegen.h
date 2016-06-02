#pragma once

#include <stack>

#include "ast_visitor.h"
#include "type.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace tiny {
	
	class CodeGen : public ASTVisitor
	{
	public:
		CodeGen(llvm::TargetMachine* tm);

		void visit(AST* ast) override;
		void visit(FnDeclaration* node) override;
		void visit(VarDeclaration* node) override;
		void visit(BinaryOperator* node) override;
		void visit(Identifier* node) override;
		void visit(IntLiteral* node) override;
		void visit(RetDeclaration* node) override;
		void visit(CallExp* node) override;

		std::unique_ptr<llvm::Module> execute(AST* ast);

	private:
		void push(llvm::Value* v);
		llvm::Value* pop();

		static llvm::Type* get_llvm_type(const TinyType* type);

		std::unique_ptr<llvm::Module> module_;
		llvm::IRBuilder<> builder_;
		std::stack<llvm::Value*> value_stack_;
	};

}