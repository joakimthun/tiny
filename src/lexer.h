#pragma once

#include <fstream>
#include <string>
#include <memory>

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
		void consume();
		void new_line();
		std::unique_ptr<Token> create(TokenType type, const std::string& value);

		std::ifstream file_;
		char current_;
		u32 line_number_;
		u32 column_;
	};

}

