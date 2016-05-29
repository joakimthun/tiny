#include <iostream>
#include <memory>

#include "parser.h"
#include "tiny_exception.h"

using namespace tiny;

int main(int argc, char* argv[])
{
	try
	{
		auto l = std::make_unique<Lexer>("test_files/test.tiny");

		auto t = l->next();
		while (t->type != TokenType::Eof)
		{
			std::cout << "Value: " << t->value << " Line: " << t->line_number << " Start: " << t->start_column << " End: " << t->end_column << std::endl;
			t = l->next();
		}

		auto p = std::make_unique<Parser>(std::make_unique<Lexer>("test_files/test.tiny"));

		auto ast = p->parse();
	}
	catch(TinyException e)
	{
		std::cout << e.what() << std::endl;

		return 1;
	}

	return 0;
}
