#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <unordered_map>

#include "type.h"
#include "token.h"

namespace tiny {

	class Lexer
	{
	public:
		Lexer(const std::string& path);
		~Lexer();
		std::unique_ptr<Token> next();
	private:
		std::unique_ptr<Token> alpha();
		void consume();
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
	};

}

