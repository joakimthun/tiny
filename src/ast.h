#pragma once

#include <vector>
#include <memory>

#include "type.h"
#include "token.h"
#include "codegen.h"
#include "symbols.h"

namespace tiny {

	enum class NodeType : u16
	{
		FnDeclaration,
		ArgDeclaration,
		VarDeclaration,
		IntLiteral,
		StringLiteral,
		BinaryOperator,
		Identifier,
		RetDeclaration,
		CallExp,
	};

	struct ASTNode
	{
		ASTNode() : type(nullptr) {}
		ASTNode(std::unique_ptr<TinyType> t) : type(std::move(t)) {}

		virtual ~ASTNode()
		{
		}

		virtual NodeType node_type() = 0;
		virtual std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) = 0;

		std::unique_ptr<TinyType> type;
	};

	struct Scope
	{
		Scope(SymbolTable* parent) : symbol_table_(std::make_unique<SymbolTable>(parent)) {}
		std::unique_ptr<SymbolTable> symbol_table_;
	};

	struct AST : Scope
	{
		AST() : Scope(nullptr) {}

		std::vector<std::unique_ptr<ASTNode>> nodes;

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor)
		{
			for (auto& n : nodes)
				n->codegen(visitor);

			return nullptr;
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

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
		}
	};

	struct FnDeclaration : ASTNode, Scope
	{
		FnDeclaration(SymbolTable* parent, bool ext) : ASTNode(std::make_unique<TinyType>(Type::Fn)), Scope(parent), entry_point(false), external(ext) {}

		std::string name;
		bool entry_point;
		std::unique_ptr<TinyType> return_type;
		std::vector<std::unique_ptr<ArgDeclaration>> args;
		std::vector<std::unique_ptr<ASTNode>> body;
		bool external;

		NodeType node_type() override
		{
			return NodeType::FnDeclaration;
		}

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
		}
	};

	struct CallExp : ASTNode
	{
		CallExp(std::unique_ptr<TinyType> t) : ASTNode(std::move(t)) {}

		std::string name;
		std::vector<std::unique_ptr<ASTNode>> args;

		NodeType node_type() override
		{
			return NodeType::CallExp;
		}

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
		}
	};

	struct VarDeclaration : ASTNode
	{
		VarDeclaration(const std::string& n, std::unique_ptr<ASTNode> exp, std::unique_ptr<TinyType> t, bool ptr) : ASTNode(std::move(t)), name(n), expression(std::move(exp)), pointer(ptr) {}
		std::string name;
		std::unique_ptr<ASTNode> expression;
		bool pointer;

		NodeType node_type() override
		{
			return NodeType::VarDeclaration;
		}

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
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

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
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

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
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

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
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

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
		}
	};

	struct StringLiteral : ASTNode
	{
		StringLiteral(const std::string& v) : ASTNode(std::make_unique<TinyType>(Type::I32)), value(v) {}

		std::string value;

		NodeType node_type() override
		{
			return NodeType::StringLiteral;
		}

		std::unique_ptr<CodegenResult> codegen(CodeGen* visitor) override
		{
			return visitor->visit(this);
		}
	};

}
