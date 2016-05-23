#include "lexer.h"
#include "tiny_exception.h"

namespace tiny {

	Lexer::Lexer(const std::string& path) : file_(path), line_number_(0), column_(0)
	{
		if (!file_.good())
		{
			throw TinyException("Could not open source file: %s", path.c_str());
		}

		consume();
	}

	Lexer::~Lexer()
	{
		if (file_.is_open())
			file_.close();
	}

	std::unique_ptr<Token> Lexer::next()
	{
		while (current_ != -1)
		{
			if(current_ == '\n')
			{
				new_line();
				continue;
			}

			if(isspace(current_))
			{
				consume();
				continue;
			}

			switch (current_)
			{
			case '+':
				return create(Plus, "+");
			case '-':
				return create(Minus, "-");
			case '=':
				return create(Assign, "=");
			case '*':
				return create(Times, "*");
			case '/':
				return create(Divide, "/");
			case '(':
				return create(LParen, "(");
			case ')':
				return create(RParen, ")");
			}

			throw TinyException("Unrecognised token Line: %d Column: %d", line_number_, column_);
		}

		return create(Eof, "");
	}

	void Lexer::consume()
	{
		if (!file_.eof())
		{
			column_++;
			current_ = file_.get();
			return;
		}

		current_ = -1;
	}

	void Lexer::new_line()
	{
		column_ = 0;
		line_number_++;
		consume();
	}

	std::unique_ptr<Token> Lexer::create(TokenType type, const std::string& value)
	{
		consume();
		return std::make_unique<Token>(type, value);
	}
}
