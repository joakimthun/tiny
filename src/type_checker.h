#pragma once

#include "ast_visitor.h"

namespace tiny {
	
	class TypeChecker : public ASTVisitor
	{
	public:
		void visit(FnDeclaration* node) override;
		void visit(VarDeclaration* node) override;
		void visit(BinaryOperator* node) override;
		void visit(Identifier* node) override;
		void visit(IntLiteral* node) override;
	};
}
