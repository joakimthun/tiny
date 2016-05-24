#pragma once

#include <memory>

#include "type.h"
#include "lexer.h"
#include "ast.h"

namespace tiny {

	class Parser
	{
	public:
		Parser(std::unique_ptr<Lexer> lexer);
		std::unique_ptr<AST> parse();
	private:
		std::unique_ptr<Lexer> lexer_;
	};
}
