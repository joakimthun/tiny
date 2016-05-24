#pragma once

#include <vector>
#include <memory>
#include <string>

#include "type.h"

class ASTVisitor;

enum class NodeType : u16
{
	FnDeclaration,
	VarDeclaration
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
