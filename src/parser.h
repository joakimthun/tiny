#pragma once

#include <memory>
#include <functional>
#include <unordered_map>

#include "type.h"
#include "lexer.h"
#include "ast.h"

namespace tiny {

	enum class Precedence : int
	{
		Assignment = 1,			// =, +=, -=, /=, :=
		ConditionalOr = 2,		// ||
		ConditionalAnd = 3,		// &&
		LogicalOr = 4,			// |
		LogicalAnd = 5,			// &
		Equality = 6,			// ==, !=
		Relational = 7,			// <, >, <=, >=
		Shift = 8,				// <<, >>
		Sum = 9,				// + , -
		Product = 10,			// * , /, %
		Unary = 11,				// ++/--
	};

	class Parser
	{
	public:
		Parser(std::unique_ptr<Lexer> lexer);
		std::unique_ptr<AST> parse();
		void consume(TokenType type);
		const Token* current() const;
		const Token* peek() const;

	private:
		void register_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler);
		void register_infix_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser, std::unique_ptr<ASTNode> left)> handler);
		void initialize();

		std::unique_ptr<Token> current_token_;
		std::unique_ptr<Lexer> lexer_;
		std::unordered_map<TokenType, std::function<std::unique_ptr<ASTNode>(Parser* parser)>> parsers_;
		std::unordered_map<TokenType, std::function<std::unique_ptr<ASTNode>(Parser* parser, std::unique_ptr<ASTNode> left)>> infix_parsers_;
	};
}
