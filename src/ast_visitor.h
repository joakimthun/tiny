#pragma once

#include <queue>

namespace tiny {

	struct AST;
	struct ASTNode;
	struct FnDeclaration;
	struct VarDeclaration;
	struct BinaryOperator;
	struct Identifier;
	struct IntLiteral;

	class ASTVisitor
	{
	public:
		virtual ~ASTVisitor() {};
		virtual void visit(AST* ast) = 0;
		virtual void visit(FnDeclaration* node) = 0;
		virtual void visit(VarDeclaration* node) = 0;
		virtual void visit(BinaryOperator* node) = 0;
		virtual void visit(Identifier* node) = 0;
		virtual void visit(IntLiteral* node) = 0;

		std::queue<ASTNode*> parents;
	};

}
