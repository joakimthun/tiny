#include "parser.h"

#include "tiny_exception.h"

namespace tiny {

	Parser::Parser(std::unique_ptr<Lexer> lexer) : lexer_(std::move(lexer))
	{
		current_token_ = lexer_->next();
	}

	std::unique_ptr<AST> Parser::parse()
	{
		return std::make_unique<AST>();
	}

	void Parser::consume(TokenType type)
	{
		if(current_token_->type != type)
			throw TinyException("Unexpected token, Line: %d Column: %d", current_token_->line_number, current_token_->start_column);

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

	void Parser::register_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler)
	{
		parsers_.insert(std::make_pair(type, handler));
	}

	void Parser::register_infix_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser, std::unique_ptr<ASTNode> left)> handler)
	{
		infix_parsers_.insert(std::make_pair(type, handler));
	}

	static std::unique_ptr<ASTNode> parse_fn_declaration(Parser* parser)
	{
		return std::make_unique<FnDeclaration>();
	}

	static std::unique_ptr<ASTNode> parse_var_declaration(Parser* parser)
	{
		return std::make_unique<VarDeclaration>();
	}

	void Parser::initialize()
	{
		// Parsers
		register_parser(TokenType::Fn, parse_fn_declaration);
		register_parser(TokenType::ShortDec, parse_var_declaration);

		// Infix parsers
	}
}
