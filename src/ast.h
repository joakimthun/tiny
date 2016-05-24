#pragma once

#include <vector>
#include <memory>

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
