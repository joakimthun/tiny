#pragma once

#include "type.h"

namespace tiny {

	enum class TokenType : u16
	{
		Eof,
		Plus,
		Minus,
		Assign,
		Times,
		RArrow,
		Divide,
		LParen,
		RParen,
		Fn,
		Ext,
		Ret,
		Id,
		IntLiteral,
		ShortDec,
		LBracket,
		RBracket,
		LSBracket,
		RSBracket,
		I32
	};

	enum class Precedence : u16
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

	inline u16 get_operator_precedence(TokenType op)
	{
		switch (op)
		{
		case TokenType::Assign:
		case TokenType::ShortDec:
			return static_cast<u16>(Precedence::Assignment);
		case TokenType::Plus:
		case TokenType::Minus:
			return static_cast<u16>(Precedence::Sum);
		case TokenType::Times:
		case TokenType::Divide:
			return static_cast<u16>(Precedence::Product);
		}

		return 0;
	}

	struct Token
	{
		Token::Token(TokenType token_type, const std::string& token_value, const std::string& file, u32 line, u32 start_col, u32 end_col) : type(token_type), value(token_value), file_name(file), line_number(line), start_column(start_col), end_column(end_col) {}

		TokenType type;
		std::string value;
		u32 line_number;
		u32 start_column;
		u32 end_column;
		std::string file_name;
	};

}