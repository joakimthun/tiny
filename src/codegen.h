#pragma once

#include <stack>

#include "ast_visitor.h"
#include "type.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

namespace tiny {
	
	class CodeGen : public ASTVisitor
	{
	public:
		CodeGen();

		void visit(AST* ast) override;
		void visit(FnDeclaration* node) override;
		void visit(VarDeclaration* node) override;
		void visit(BinaryOperator* node) override;
		void visit(Identifier* node) override;
		void visit(IntLiteral* node) override;
		void visit(RetDeclaration* node) override;
		void dump_module() const;
	private:
		static std::unique_ptr<llvm::IntegerType> get_llvm_type(const TinyType* type);

		llvm::IRBuilder<> builder_;
		std::unique_ptr<llvm::Module> module_;
		std::stack<llvm::Value*> value_stack_;
	};

}