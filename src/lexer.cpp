#include "lexer.h"
#include "tiny_exception.h"

namespace tiny {

	Lexer::Lexer(const std::string& path) : file_(path), line_number_(0), column_(0)
	{
	}

	Lexer::~Lexer()
	{
		if (file_.is_open())
			file_.close();
	}

	Token Lexer::next_token()
	{
		const auto c = next_char();

		if (c == 0)
			return Token::Eof;

		switch (c)
		{
		case '+':
			return Token::Plus;
		case '-':
			return Token::Minus;
		case '=':
			return Token::Assign;
		case '*':
			return Token::Times;
		case '/':
			return Token::Divide;
		case '(':
			return Token::LParen;
		case ')':
			return Token::RParen;
		}

		throw TinyException("Unrecognised token Line: %d Column: %d", line_number_, column_);
	}

	char Lexer::next_char()
	{
		if (file_.is_open() && file_.good())
		{
			column_++;
			return file_.get();
		}

		return 0;
	}
}
