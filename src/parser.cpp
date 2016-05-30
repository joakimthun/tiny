#include <sstream>
#include <iostream>

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

		push_scope(ast->symbol_table_.get());

		while (current_token_->type != TokenType::Eof)
		{
			ast->nodes.push_back(parse_global());
		}

		pop_scope();

		throw_if_has_errors();

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
		auto parser = get_ll2_parser(current_token_->type, peek()->type);

		if (parser == nullptr)
		{
			parser = get_parser(current_token_->type);
			if (parser == nullptr)
				throw_unexpected_token();
		}

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

	bool Parser::consume_ptr()
	{
		if(current_token_->type == TokenType::Star)
		{
			consume(TokenType::Star);
			return true;
		}

		return false;
	}

	const Token* Parser::current() const
	{
		return current_token_.get();
	}

	const Token* Parser::peek() const
	{
		return lexer_->peek();
	}

	void Parser::register_error(const std::string& msg)
	{
		errors_.push_back(msg);

		if(errors_.size() >= 5)
		{
			throw_if_has_errors();
		}
	}

	void Parser::push_scope(SymbolTable* scope)
	{
		scopes_.push(scope);
	}

	void Parser::pop_scope()
	{
		scopes_.pop();
	}

	SymbolTable* Parser::current_scope()
	{
		return scopes_.top();
	}

	void Parser::throw_if_has_errors() const
	{
		if(errors_.size() == 0)
			return;

		auto s = std::stringstream();

		s << "Errors: " << std::endl;

		for (const auto& msg : errors_)
			s << msg << std::endl;

		throw TinyException(s.str());
	}

	void Parser::throw_unexpected_token() const
	{
		throw TinyException("Unexpected token, Line: " + std::to_string(current_token_->line_number) + " Column: " + std::to_string(current_token_->start_column));
	}

	void Parser::register_global_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler)
	{
		global_parsers_.insert(std::make_pair(type, handler));
	}

	void Parser::register_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler)
	{
		parsers_.insert(std::make_pair(type, handler));
	}

	void Parser::register_ll2_parser(TokenType t1, TokenType t2, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler)
	{
		ll2_parsers_.push_back(LL2ParserEntry{ t1, t2, handler });
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

	std::function<std::unique_ptr<ASTNode>(Parser*)> Parser::get_ll2_parser(TokenType t1, TokenType t2)
	{
		for(auto& e : ll2_parsers_)
		{
			if (e.t1 == t1 && e.t2 == t2)
				return e.parser;
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
		register_global_parser(TokenType::Ext, parse_fn_declaration);

		// LL2 parsers
		register_ll2_parser(TokenType::Id, TokenType::ShortDec, parse_short_dec);
		register_ll2_parser(TokenType::Id, TokenType::LParen, parse_call);

		// Parsers
		register_parser(TokenType::LParen, parse_grouped_expression);
		register_parser(TokenType::Id, parse_id);
		register_parser(TokenType::IntLiteral, parse_literal);
		register_parser(TokenType::Ret, parse_ret_dec);

		// Infix parsers
		register_infix_parser(TokenType::Assign, parse_binary_operator);
		register_infix_parser(TokenType::ShortDec, parse_binary_operator);
		register_infix_parser(TokenType::Plus, parse_binary_operator);
		register_infix_parser(TokenType::Minus, parse_binary_operator);
		register_infix_parser(TokenType::Star, parse_binary_operator);
		register_infix_parser(TokenType::Divide, parse_binary_operator);
	}
}
