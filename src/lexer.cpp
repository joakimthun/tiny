#include "lexer.h"
#include "tiny_exception.h"

namespace tiny {

	Lexer::Lexer(const std::string& path) : path_(path), file_(path), line_number_(1), column_(0)
	{
		if (!file_.good())
		{
			throw TinyException("Could not open source file: %s", path.c_str());
		}

		consume();
		init_keywords();
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

			if (isalpha(current_))
			{
				return alpha();
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

	std::unique_ptr<Token> Lexer::alpha()
	{
		std::string value;
		u32 start = column_;
		u32 end = column_;
		while (current_ != -1 && (isalnum(current_) || current_ == L'_'))
		{
			value += current_;
			end++;
			consume();
		}

		auto t = match_keyword(value, start, end);
		if (t != nullptr)
			return t;

		return std::make_unique<Token>(Id, value, path_, line_number_, start, end);
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
		return create(type, value, column_, column_);
	}

	std::unique_ptr<Token> Lexer::create(TokenType type, const std::string& value, u32 start_col, u32 end_col)
	{
		consume();
		return std::make_unique<Token>(type, value, path_, line_number_, start_col, end_col);
	}

	std::unique_ptr<Token> Lexer::match_keyword(const std::string& value, u32 start_col, u32 end_col)
	{
		auto it = keywords_.find(value);
		if (it != keywords_.end())
		{
			const auto& t = it->second;
			return std::make_unique<Token>(t->type, value, path_, line_number_, start_col, end_col);
		}

		return nullptr;
	}

	void Lexer::register_keyword(const std::string keyword, TokenType type)
	{
		keywords_.insert(std::pair<std::string, std::unique_ptr<Token>>(keyword, std::make_unique<Token>(type, keyword, path_, line_number_, 0, 0)));
	}

	void Lexer::init_keywords()
	{
		register_keyword("fn", Fn);
	}
}
