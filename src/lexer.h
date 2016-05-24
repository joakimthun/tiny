#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <unordered_map>
#include <queue>

#include "type.h"
#include "token.h"

namespace tiny {

	class Lexer
	{
	public:
		Lexer(const std::string& path);
		~Lexer();
		std::unique_ptr<Token> next();
		const Token* peek();
	private:
		std::unique_ptr<Token> alpha();
		std::unique_ptr<Token> digit();
		std::unique_ptr<Token> try_match_tokens(char first, char second, TokenType type);
		void consume();
		char peek_char();
		void new_line();
		std::unique_ptr<Token> create(TokenType type, const std::string& value);
		std::unique_ptr<Token> create(TokenType type, const std::string& value, u32 start_col, u32 end_col);
		std::unique_ptr<Token> match_keyword(const std::string& value, u32 start_col, u32 end_col);
		void register_keyword(const std::string keyword, TokenType type);
		void init_keywords();

		std::string path_;
		std::ifstream file_;
		char current_;
		u32 line_number_;
		u32 column_;
		std::unordered_map<std::string, std::unique_ptr<Token>> keywords_;
		std::queue<std::unique_ptr<Token>> buffer_;
	};

}

