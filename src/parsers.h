#pragma once

#include <memory>

namespace tiny {

	struct ASTNode;
	class Parser;

	std::unique_ptr<ASTNode> parse_fn_declaration(Parser* parser);
	std::unique_ptr<ASTNode> parse_id(Parser* parser);
	std::unique_ptr<ASTNode> parse_literal(Parser* parser);
	std::unique_ptr<ASTNode> parse_binary_operator(Parser* parser, std::unique_ptr<ASTNode> left);
	std::unique_ptr<ASTNode> parse_grouped_expression(Parser* parser);
	std::unique_ptr<ASTNode> parse_short_dec(Parser* parser);
	std::unique_ptr<ASTNode> parse_explicit_dec(Parser* parser);
	std::unique_ptr<ASTNode> parse_ret_dec(Parser* parser);
	std::unique_ptr<ASTNode> parse_call(Parser* parser);
	
}