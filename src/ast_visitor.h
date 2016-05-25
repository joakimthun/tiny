#pragma once

#include "ast.h"

namespace tiny {

	class ASTVisitor
	{
	public:
		virtual ~ASTVisitor() {};
		virtual void visit(FnDeclaration* node) = 0;
		virtual void visit(VarDeclaration* node) = 0;
		virtual void visit(BinaryOperator* node) = 0;
		virtual void visit(Identifier* node) = 0;
		virtual void visit(IntLiteral* node) = 0;
	};

}
