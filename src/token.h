#pragma once

#include "type.h"

enum TokenType : u16
{
	Eof,
	Plus,
	Minus,
	Assign,
	Times,
	Divide,
	LParen,
	RParen,
	Fn,
	Id,
	Int32,
	ShortDec
};

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