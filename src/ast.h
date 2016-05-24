#pragma once

#include <vector>
#include <memory>
#include <string>

#include "type.h"

class ASTVisitor;

struct ASTNode
{
	virtual ~ASTNode()
	{
	}

	virtual void accept(ASTVisitor* visitor) = 0;
};

struct AST
{
	std::vector<std::unique_ptr<ASTNode>> nodes;
};

struct FnDeclaration : ASTNode
{
	std::string name;
	std::vector<Type> return_types;
	std::vector<std::unique_ptr<ASTNode>> body;
};
