#include "parser.h"

namespace tiny {

	Parser::Parser(std::unique_ptr<Lexer> lexer) : lexer_(std::move(lexer))
	{
		
	}

	std::unique_ptr<AST> Parser::parse()
	{
		return std::make_unique<AST>();
	}
}
