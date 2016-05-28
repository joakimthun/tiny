#include "ast.h"
#include "token.h"
#include "parser.h"
#include "tiny_exception.h"

namespace tiny {

	std::unique_ptr<ASTNode> parse_fn_declaration(Parser* parser)
	{
		auto fn = std::make_unique<FnDeclaration>();
		parser->consume(TokenType::Fn);

		auto name = parser->current()->value;
		parser->consume(TokenType::Id);
		fn->name = name;

		parser->consume(TokenType::LParen);
		parser->consume(TokenType::RParen);

		parser->consume(TokenType::RArrow);

		// TODO: Grab the return type...
		parser->consume();

		parser->consume(TokenType::LBracket);

		while (parser->current()->type != TokenType::RBracket)
		{
			fn->body.push_back(parser->parse_expression());
		}

		parser->consume(TokenType::RBracket);

		return std::move(fn);
	}

	std::unique_ptr<ASTNode> parse_id(Parser* parser)
	{
		auto name = parser->current()->value;
		parser->consume(TokenType::Id);
		return std::make_unique<Identifier>(name);
	}

	std::unique_ptr<ASTNode> parse_literal(Parser* parser)
	{
		auto value = parser->current()->value;
		switch (parser->current()->type)
		{
		case TokenType::IntLiteral: {
			parser->consume(TokenType::IntLiteral);
			return std::make_unique<IntLiteral>(std::stoi(value));
		}
		default:
			throw TinyException("Unexpected literal -> parse_literal");
		}
	}

	std::unique_ptr<ASTNode> parse_binary_operator(Parser* parser, std::unique_ptr<ASTNode> left)
	{
		auto binary_node = std::make_unique<BinaryOperator>();

		binary_node->op = parser->current()->type;
		parser->consume();

		binary_node->left = std::move(left);
		binary_node->right = parser->parse_expression(get_operator_precedence(binary_node->op));

		return std::move(binary_node);
	}

	std::unique_ptr<ASTNode> parse_grouped_expression(Parser* parser)
	{
		parser->consume(TokenType::LParen);
		auto expression = parser->parse_expression();
		parser->consume(TokenType::RParen);
		return expression;
	}

}