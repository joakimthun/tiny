#include "ast.h"
#include "token.h"
#include "parser.h"
#include "tiny_exception.h"
#include "type.h"

namespace tiny {

	std::unique_ptr<ASTNode> parse_fn_declaration(Parser* parser)
	{
		auto fn = std::make_unique<FnDeclaration>(parser->current_scope());
		parser->push_scope(fn->symbol_table_.get());
		parser->consume(TokenType::Fn);

		auto name = parser->current()->value;
		parser->consume(TokenType::Id);
		fn->name = name;

		parser->consume(TokenType::LParen);
		parser->consume(TokenType::RParen);
		parser->consume(TokenType::RArrow);

		fn->return_type = get_type_from_token(parser->current()->type);
		parser->consume();

		parser->consume(TokenType::LBracket);

		while (parser->current()->type != TokenType::RBracket)
		{
			fn->body.push_back(parser->parse_expression());
		}

		parser->consume(TokenType::RBracket);

		parser->pop_scope();

		return std::move(fn);
	}

	std::unique_ptr<ASTNode> parse_id(Parser* parser)
	{
		auto name = parser->current()->value;
		parser->consume(TokenType::Id);

		if (parser->current_scope()->has_entry(name))
		{
			auto entry = parser->current_scope()->get_entry(name);
			return std::make_unique<Identifier>(name, std::make_unique<TinyType>(entry->type->type));
		}

		parser->register_error("Unknown identifier '" + name + "', line: " + std::to_string(parser->current()->line_number));
		return std::make_unique<Identifier>(name, std::make_unique<TinyType>(Type::Undefined));
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
		auto op = parser->current()->type;
		parser->consume();

		auto right = parser->parse_expression(get_operator_precedence(op));

		if (!left->type->are_equal(right->type.get()))
			parser->register_error("Type mismatch at line: " + std::to_string(parser->current()->line_number));

		return std::make_unique<BinaryOperator>(op, std::move(left), std::move(right));
	}

	std::unique_ptr<ASTNode> parse_grouped_expression(Parser* parser)
	{
		parser->consume(TokenType::LParen);
		auto expression = parser->parse_expression();
		parser->consume(TokenType::RParen);
		return expression;
	}

	std::unique_ptr<ASTNode> parse_short_dec(Parser* parser)
	{
		auto name = parser->current()->value;
		parser->consume(TokenType::Id);
		parser->consume(TokenType::ShortDec);

		auto exp = parser->parse_expression();

		if (parser->current_scope()->has_entry(name))
			parser->register_error("An identifier with the name '" + name + "' already exists in the current scope, Line: " + std::to_string(parser->current()->line_number));
		else
			parser->current_scope()->add_entry(name, std::make_unique<TinyType>(exp->type->type));

		return std::make_unique<VarDeclaration>(name, std::move(exp), std::make_unique<TinyType>(exp->type->type));
	}

}