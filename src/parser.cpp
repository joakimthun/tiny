#include "parser.h"

namespace tiny {

	Parser::Parser(std::unique_ptr<Lexer> lexer) : lexer_(std::move(lexer))
	{
		
	}
}