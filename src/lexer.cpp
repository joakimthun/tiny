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
		if (!buffer_.empty())
		{
			auto next = std::move(buffer_.front());
			buffer_.pop();
			return next;
		}

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

			if (isdigit(current_))
			{
				return digit();
			}

			switch (current_)
			{
			case '+':
				return create(TokenType::Plus, "+");
			case '-': {
				auto t = try_match_tokens('-', '>', TokenType::RArrow);
				if (t != nullptr)
					return t;

				return create(TokenType::Minus, "-");
			}
			case '=':
				return create(TokenType::Assign, "=");
			case '*':
				return create(TokenType::Times, "*");
			case '/':
				return create(TokenType::Divide, "/");
			case '(':
				return create(TokenType::LParen, "(");
			case ')':
				return create(TokenType::RParen, ")");
			case '{':
				return create(TokenType::LBracket, "{");
			case '}':
				return create(TokenType::RBracket, "}");
			case '[':
				return create(TokenType::LSBracket, "[");
			case ']':
				return create(TokenType::RSBracket, "]");
			case ':': {
				auto t = try_match_tokens(':', '=', TokenType::ShortDec);
				if (t != nullptr)
					return t;
			}
			}

			throw TinyException("Unrecognised token, Line: %d Column: %d", line_number_, column_);
		}

		return create(TokenType::Eof, "");
	}

	const Token* Lexer::peek()
	{
		buffer_.push(next());
		return buffer_.back().get();
	}

	std::unique_ptr<Token> Lexer::alpha()
	{
		std::string value;
		u32 start = column_;
		while (current_ != -1 && (isalnum(current_) || current_ == L'_'))
		{
			value += current_;
			consume();
		}

		auto t = match_keyword(value, start, start + (value.size() - 1));
		if (t != nullptr)
			return t;

		return std::make_unique<Token>(TokenType::Id, value, path_, line_number_, start, start + (value.size() - 1));
	}

	std::unique_ptr<Token> Lexer::digit()
	{
		std::string value;
		u32 start = column_;

		while (current_ != -1 && isdigit(current_))
		{
			value += current_;
			consume();
		}

		return std::make_unique<Token>(TokenType::IntLiteral, value, path_, line_number_, start, start + (value.size() - 1));
	}

	std::unique_ptr<Token> Lexer::try_match_tokens(char first, char second, TokenType type)
	{
		if (current_ == first && peek_char() == second)
		{
			u32 start = column_;
			std::string value;
			value += current_;
			consume();
			value += current_;

			return create(type, value, start, start + 1);
		}

		return nullptr;
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

	char Lexer::peek_char()
	{
		if (!file_.eof())
		{
			return file_.peek();
		}

		return -1;
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
		register_keyword("fn", TokenType::Fn);

		// Types
		register_keyword("i32", TokenType::Fn);
	}
}
