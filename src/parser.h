#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <stack>

#include "type.h"
#include "lexer.h"
#include "ast.h"

namespace tiny {

	class Parser;

	struct LL2ParserEntry
	{
		TokenType t1;
		TokenType t2;
		std::function<std::unique_ptr<ASTNode>(Parser*)> parser;
	};

	class Parser
	{
	public:
		Parser(std::unique_ptr<Lexer> lexer);
		std::unique_ptr<AST> parse();
		std::unique_ptr<ASTNode> parse_global();
		std::unique_ptr<ASTNode> parse_expression();
		std::unique_ptr<ASTNode> parse_expression(u16 precedence);
		void consume(TokenType type);
		void consume();
		const Token* current() const;
		const Token* peek() const;
		void register_error(const std::string& msg);
		void push_scope(SymbolTable* scope);
		void pop_scope();
		SymbolTable* current_scope();

	private:
		void throw_if_has_errors() const;
		void throw_unexpected_token() const;

		void register_global_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler);
		void register_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler);
		void register_ll2_parser(TokenType t1, TokenType t2, std::function<std::unique_ptr<ASTNode>(Parser* parser)> handler);
		void register_infix_parser(TokenType type, std::function<std::unique_ptr<ASTNode>(Parser* parser, std::unique_ptr<ASTNode> left)> handler);
		std::function<std::unique_ptr<ASTNode>(Parser*)> get_global_parser(TokenType type);
		std::function<std::unique_ptr<ASTNode>(Parser*)> get_parser(TokenType type);
		std::function<std::unique_ptr<ASTNode>(Parser*)> get_ll2_parser(TokenType t1, TokenType t2);
		std::function<std::unique_ptr<ASTNode>(Parser*, std::unique_ptr<ASTNode>)> get_infix_parser(TokenType type);

		void initialize_grammar();

		std::unique_ptr<Token> current_token_;
		std::unique_ptr<Lexer> lexer_;
		std::stack<SymbolTable*> scopes_;
		std::unordered_map<TokenType, std::function<std::unique_ptr<ASTNode>(Parser*)>> global_parsers_;
		std::unordered_map<TokenType, std::function<std::unique_ptr<ASTNode>(Parser*)>> parsers_;
		std::vector<LL2ParserEntry> ll2_parsers_;
		std::unordered_map<TokenType, std::function<std::unique_ptr<ASTNode>(Parser*, std::unique_ptr<ASTNode>)>> infix_parsers_;
		std::vector<std::string> errors_;
	};
}
