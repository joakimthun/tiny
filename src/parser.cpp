#include "parser.h"

#include "tiny_exception.h"
#include "parsers.h"

namespace tiny {

	Parser::Parser(std::unique_ptr<Lexer> lexer) : lexer_(std::move(lexer))
	{
		current_token_ = lexer_->next();
		initialize_grammar();
	}

	std::unique_ptr<AST> Parser::parse()
	{
		auto ast = std::make_unique<AST>();
		while (current_token_->type != TokenType::Eof)
		{
			ast->nodes.push_back(parse_global());
		}

		return ast;
	}

	std::unique_ptr<ASTNode> Parser::parse_global()
	{
		auto parser = get_global_parser(current_token_->type);
		if(parser == nullptr)
			throw_unexpected_token();

		return parser(this);
	}

	std::unique_ptr<ASTNode> Parser::parse_expression()
	{
		return parse_expression(0);
	}

	std::unique_ptr<ASTNode> Parser::parse_expression(u16 precedence)
	{
		auto parser = get_parser(current_token_->type);
		if (parser == nullptr)
			throw_unexpected_token();

		auto left = parser(this);
		while (precedence < get_operator_precedence(current_token_->type))
		{
			auto infix_parser = get_infix_parser(current_token_->type);
			if (infix_parser == nullptr)
				throw_unexpected_token();

			left = infix_parser(this, std::move(left));
		}

		return left;
	}

	void Parser::consume(TokenType type)
	{
		if (current_token_->type != type)
			throw_unexpected_token();

		current_token_ = lexer_->next();
	}

	void Parser::consume()
	{
		current_token_ = lexer_->next();
	}

	const Token* Parser::current() const
	{
		return current_token_.get();
	}

	const Token* Parser::peek() const
	{
		return lexer_->peek();
	}

	void Parser::throw_unexpected_token() const
	{
		throw TinyException("Unexpected token, Line: %d Column: %d", current_token_->line_number, current_token_->start_column);
	}

	void Parser::register_global_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler)
	{
		global_parsers_.insert(std::make_pair(type, handler));
	}

	void Parser::register_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler)
	{
		parsers_.insert(std::make_pair(type, handler));
	}

	void Parser::register_infix_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser, std::unique_ptr<ASTNode> left)> handler)
	{
		infix_parsers_.insert(std::make_pair(type, handler));
	}

	std::function<std::unique_ptr<ASTNode>(Parser*)> Parser::get_global_parser(TokenType type)
	{
		auto it = global_parsers_.find(type);
		if (it != global_parsers_.end())
		{
			return it->second;
		}

		return nullptr;
	}

	std::function<std::unique_ptr<ASTNode>(Parser*)> Parser::get_parser(TokenType type)
	{
		auto it = parsers_.find(type);
		if (it != parsers_.end())
		{
			return it->second;
		}

		return nullptr;
	}

	std::function<std::unique_ptr<ASTNode>(Parser*, std::unique_ptr<ASTNode>)> Parser::get_infix_parser(TokenType type)
	{
		auto it = infix_parsers_.find(type);
		if (it != infix_parsers_.end())
		{
			return it->second;
		}

		return nullptr;
	}

	

	void Parser::initialize_grammar()
	{
		// Global parsers
		register_global_parser(TokenType::Fn, parse_fn_declaration);
		register_global_parser(TokenType::Id, parse_id);

		// Parsers
		register_parser(TokenType::LParen, parse_grouped_expression);
		register_parser(TokenType::Id, parse_id);
		register_parser(TokenType::IntLiteral, parse_literal);

		// Infix parsers
		register_infix_parser(TokenType::Assign, parse_binary_operator);
		register_infix_parser(TokenType::ShortDec, parse_binary_operator);
		register_infix_parser(TokenType::Plus, parse_binary_operator);
		register_infix_parser(TokenType::Minus, parse_binary_operator);
		register_infix_parser(TokenType::Times, parse_binary_operator);
		register_infix_parser(TokenType::Divide, parse_binary_operator);
	}
}
