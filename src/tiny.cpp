#include <iostream>
#include <memory>

#include "lexer.h"

using namespace tiny;

int main(int argc, char* argv[])
{
	auto l = std::make_unique<Lexer>("test_files/test.tiny");

	auto t = l->next();
	while(t->type != Eof)
	{
		std::cout << "Value: " << t->value << " Line: " << t->line_number << " Start: " << t->start_column << " End: " << t->end_column << std::endl;
		t = l->next();
	}
}
