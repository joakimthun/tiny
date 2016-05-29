#pragma once

#include <vector>
#include <memory>

#include "type.h"
#include "token.h"
#include "ast_visitor.h"
#include "symbols.h"

#define VISIT(visitor) \
	visitor->parents.push(this); \
	visitor->visit(this); \
	visitor->parents.pop(); \

namespace tiny {

	enum class NodeType : u16
	{
		FnDeclaration,
		ArgDeclaration,
		VarDeclaration,
		IntLiteral,
		BinaryOperator,
		Identifier,
		RetDeclaration
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

	struct Scope
	{
		Scope(const SymbolTable* parent) : symbol_table_(std::make_unique<SymbolTable>(parent)) {}
		std::unique_ptr<SymbolTable> symbol_table_;
	};

	struct AST : Scope
	{
		AST() : Scope(nullptr) {}

		std::vector<std::unique_ptr<ASTNode>> nodes;

		void accept(ASTVisitor* visitor)
		{
			for (auto& n : nodes)
				n->accept(visitor);
		}
	};

	struct ArgDeclaration : ASTNode
	{
		ArgDeclaration(const std::string& n, std::unique_ptr<TinyType> t) : ASTNode(std::move(t)), name(n) {}

		std::string name;

		NodeType node_type() override
		{
			return NodeType::ArgDeclaration;
		}

		void accept(ASTVisitor* visitor) override
		{
		}
	};

	struct FnDeclaration : ASTNode, Scope
	{
		FnDeclaration(const SymbolTable* parent) : ASTNode(std::make_unique<TinyType>(Type::Fn)), Scope(parent), entry_point(false) {}

		std::string name;
		bool entry_point;
		std::unique_ptr<TinyType> return_type;
		std::vector<std::unique_ptr<ArgDeclaration>> args;
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
		VarDeclaration(const std::string& n, std::unique_ptr<ASTNode> exp, std::unique_ptr<TinyType> t) : ASTNode(std::move(t)), name(n), expression(std::move(exp)) {}
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

	struct RetDeclaration : ASTNode
	{
		RetDeclaration(std::unique_ptr<ASTNode> exp) : ASTNode(std::make_unique<TinyType>(exp->type->type)), expression(std::move(exp)) {}
		std::unique_ptr<ASTNode> expression;

		NodeType node_type() override
		{
			return NodeType::RetDeclaration;
		}

		void accept(ASTVisitor* visitor) override
		{
			VISIT(visitor)
		}
	};

	struct BinaryOperator : ASTNode
	{
		BinaryOperator(TokenType o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) : ASTNode(std::make_unique<TinyType>(l->type->type)), op(o), left(std::move(l)), right(std::move(r)) {}
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
		Identifier(const std::string& n, std::unique_ptr<TinyType> t) : ASTNode(std::move(t)), name(n) {}

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
