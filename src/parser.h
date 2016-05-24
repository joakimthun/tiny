#pragma once

#include <memory>

#include "type.h"
#include "lexer.h"

namespace tiny {

	class Parser
	{
	public:
		Parser(std::unique_ptr<Lexer> lexer);

	private:
		std::unique_ptr<Lexer> lexer_;
	};
}
