#pragma once

#include <fstream>
#include <string>

#include "type.h"
#include "token.h"

namespace tiny {

	class Lexer
	{
	public:
		Lexer(const std::string& path);
		~Lexer();
		Token next_token();

	private:
		char next_char();

		std::ifstream file_;
		u32 line_number_;
		u32 column_;
	};

}

