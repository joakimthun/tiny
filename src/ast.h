#pragma once

#include <vector>
#include <memory>

#include "type.h"
#include "token.h"
#include "ast_visitor.h"

#define VISIT(visitor) \
	visitor->parents.push(this); \
	visitor->visit(this); \
	visitor->parents.pop(); \

namespace tiny {

	enum class NodeType : u16
	{
		FnDeclaration,
		VarDeclaration,
		IntLiteral,
		BinaryOperator,
		Identifier,
	};

	struct ASTNode
	{
		ASTNode() : type(nullptr) {}
		ASTNode(std::unique_ptr<TinyType> t) : type(std::move(t)) {}

		virtual ~ASTNode()
		{
		}

		virtual NodeType node_type() = 0;
		virtual void accept(ASTVisitor* visitor) = 0;

		std::unique_ptr<TinyType> type;
	};

	struct AST
	{
		std::vector<std::unique_ptr<ASTNode>> nodes;

		void accept(ASTVisitor* visitor)
		{
			for (auto& n : nodes)
				n->accept(visitor);
		}
	};

	struct FnDeclaration : ASTNode
	{
		FnDeclaration() : ASTNode(std::make_unique<TinyType>(Type::Fn)), entry_point(false) {}

		std::string name;
		bool entry_point;
		std::unique_ptr<TinyType> return_type;
		std::vector<std::unique_ptr<ASTNode>> body;

		NodeType node_type() override
		{
			return NodeType::FnDeclaration;
		}

		void accept(ASTVisitor* visitor) override 
		{
			VISIT(visitor)
		}
	};

	struct VarDeclaration : ASTNode
	{
		std::string name;
		std::unique_ptr<ASTNode> expression;

		NodeType node_type() override
		{
			return NodeType::VarDeclaration;
		}

		void accept(ASTVisitor* visitor) override 
		{
			VISIT(visitor)
		}
	};

	struct BinaryOperator : ASTNode
	{
		TokenType op;
		std::unique_ptr<ASTNode> left;
		std::unique_ptr<ASTNode> right;

		NodeType node_type() override
		{
			return NodeType::BinaryOperator;
		}

		void accept(ASTVisitor* visitor) override 
		{
			VISIT(visitor)
		}
	};

	struct Identifier : ASTNode
	{
		Identifier(const std::string& n) : name(n) {}

		std::string name;

		NodeType node_type() override
		{
			return NodeType::Identifier;
		}

		void accept(ASTVisitor* visitor) override 
		{
			VISIT(visitor)
		}
	};

	struct IntLiteral : ASTNode
	{
		IntLiteral(i32 v) : ASTNode(std::make_unique<TinyType>(Type::I32)), value(v) {}

		i32 value;

		NodeType node_type() override
		{
			return NodeType::IntLiteral;
		}

		void accept(ASTVisitor* visitor) override 
		{
			VISIT(visitor)
		}
	};

}
