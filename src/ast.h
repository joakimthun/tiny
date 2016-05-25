#pragma once

#include <vector>
#include <memory>
#include <string>

#include "type.h"

namespace tiny {

	class ASTVisitor;

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
		virtual ~ASTNode()
		{
		}

		virtual NodeType node_type() = 0;
		virtual void accept(ASTVisitor* visitor) = 0;
	};

	struct AST
	{
		std::vector<std::unique_ptr<ASTNode>> nodes;
	};

	struct FnDeclaration : ASTNode
	{
		FnDeclaration() : entry_point(false) {}

		std::string name;
		bool entry_point;
		std::vector<Type> return_types;
		std::vector<std::unique_ptr<ASTNode>> body;

		NodeType node_type() override
		{
			return NodeType::FnDeclaration;
		}

		void accept(ASTVisitor* visitor) override {}
	};

	struct VarDeclaration : ASTNode
	{
		std::string name;
		Type type;
		std::unique_ptr<ASTNode> expression;

		NodeType node_type() override
		{
			return NodeType::VarDeclaration;
		}

		void accept(ASTVisitor* visitor) override {}
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

		void accept(ASTVisitor* visitor) override {}
	};

	struct Identifier : ASTNode
	{
		Identifier(const std::string& n) : name(n) {}

		std::string name;

		NodeType node_type() override
		{
			return NodeType::Identifier;
		}

		void accept(ASTVisitor* visitor) override {}
	};

	struct IntLiteral : ASTNode
	{
		IntLiteral(i32 v) : value(v) {}

		i32 value;

		NodeType node_type() override
		{
			return NodeType::IntLiteral;
		}

		void accept(ASTVisitor* visitor) override {}
	};

}
