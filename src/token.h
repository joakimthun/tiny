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
	RParen
};

struct Token
{
	Token::Token(TokenType token_type, const std::string& token_value) : type(token_type), value(token_value) {}
	Token::Token(TokenType token_type) : type(token_type) {}

	TokenType type;
	std::string value;
};