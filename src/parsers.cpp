#include "ast.h"
#include "token.h"
#include "parser.h"
#include "tiny_exception.h"
#include "type.h"

namespace tiny {

	std::unique_ptr<ASTNode> parse_fn_declaration(Parser* parser)
	{
		bool ext = false;
		if(parser->current()->type == TokenType::Ext)
		{
			ext = true;
			parser->consume(TokenType::Ext);
		}

		auto fn = std::make_unique<FnDeclaration>(parser->current_scope(), ext);
		parser->push_scope(fn->symbol_table_.get());
		parser->consume(TokenType::Fn);

		auto name = parser->current()->value;
		parser->consume(TokenType::Id);
		fn->name = name;

		parser->consume(TokenType::LParen);

		while (parser->current()->type != TokenType::RParen)
		{
			auto arg_name = parser->current()->value;
			parser->consume(TokenType::Id);
			auto arg_type_token = parser->current()->type;
			parser->consume();

			auto pointer = parser->consume_ptr();
			auto arg_type = get_type_from_token(arg_type_token, pointer);
			

			fn->args.push_back(std::make_unique<ArgDeclaration>(arg_name, std::move(arg_type)));
		}

		parser->consume(TokenType::RParen);
		parser->consume(TokenType::RArrow);

		auto return_type_token = parser->current()->type;
		parser->consume();

		auto pointer = parser->consume_ptr();
		fn->return_type = get_type_from_token(return_type_token, pointer);

		parser->current_scope()->add_root_entry(name, std::make_unique<TinyType>(Type::Fn));

		if(ext)
			return std::move(fn);

		parser->consume(TokenType::LBracket);

		while (parser->current()->type != TokenType::RBracket)
		{
			fn->body.push_back(parser->parse_expression());
		}

		parser->consume(TokenType::RBracket);

		parser->pop_scope();

		return std::move(fn);
	}

	std::unique_ptr<ASTNode> parse_id(Parser* parser)
	{
		auto name = parser->current()->value;
		parser->consume(TokenType::Id);

		if (parser->current_scope()->has_entry(name))
		{
			auto entry = parser->current_scope()->get_entry(name);
			return std::make_unique<Identifier>(name, std::make_unique<TinyType>(entry->value->type));
		}

		parser->register_error("Unknown identifier '" + name + "', line: " + std::to_string(parser->current()->line_number));
		return std::make_unique<Identifier>(name, std::make_unique<TinyType>(Type::Unresolved));
	}

	std::unique_ptr<ASTNode> parse_literal(Parser* parser)
	{
		auto value = parser->current()->value;
		switch (parser->current()->type)
		{
		case TokenType::IntLiteral: {
			parser->consume(TokenType::IntLiteral);
			return std::make_unique<IntLiteral>(std::stoi(value));
		}
		case TokenType::StringLiteral: {
			parser->consume(TokenType::StringLiteral);
			return std::make_unique<StringLiteral>(value);
		}
		default:
			throw TinyException("Unexpected literal -> parse_literal");
		}
	}

	std::unique_ptr<ASTNode> parse_binary_operator(Parser* parser, std::unique_ptr<ASTNode> left)
	{
		auto op = parser->current()->type;
		parser->consume();

		auto right = parser->parse_expression(get_operator_precedence(op));

		if (!left->type->are_equal(right->type.get()))
			parser->register_error("Type mismatch at line: " + std::to_string(parser->current()->line_number));

		return std::make_unique<BinaryOperator>(op, std::move(left), std::move(right));
	}

	std::unique_ptr<ASTNode> parse_grouped_expression(Parser* parser)
	{
		parser->consume(TokenType::LParen);
		auto expression = parser->parse_expression();
		parser->consume(TokenType::RParen);
		return expression;
	}

	std::unique_ptr<ASTNode> parse_dec(Parser* parser, const std::string& name, std::unique_ptr<TinyType> type, std::unique_ptr<ASTNode> exp, bool pointer = false)
	{
		if (parser->current_scope()->has_entry(name))
			parser->register_error("An identifier with the name '" + name + "' already exists in the current scope, Line: " + std::to_string(parser->current()->line_number));
		else
			parser->current_scope()->add_entry(name, std::make_unique<TinyType>(exp->type->type));

		return std::make_unique<VarDeclaration>(name, std::move(exp), std::move(type), pointer);
	}

	std::unique_ptr<ASTNode> parse_short_dec(Parser* parser)
	{
		auto name = parser->current()->value;
		parser->consume(TokenType::Id);
		parser->consume(TokenType::ShortDec);

		auto exp = parser->parse_expression();
		auto type = exp->type->type;

		return parse_dec(parser, name, std::make_unique<TinyType>(type), std::move(exp));
	}

	std::unique_ptr<ASTNode> parse_explicit_dec(Parser* parser)
	{
		auto type_token = parser->current()->type;
		parser->consume();
		
		auto pointer = parser->consume_ptr();

		auto type = get_type_from_token(type_token, pointer);

		auto name = parser->current()->value;
		parser->consume(TokenType::Id);
		parser->consume(TokenType::Assign);

		auto exp = parser->parse_expression();

		return  parse_dec(parser, name, std::move(type), std::move(exp), pointer);
	}

	std::unique_ptr<ASTNode> parse_ret_dec(Parser* parser)
	{
		parser->consume(TokenType::Ret);
		return std::make_unique<RetDeclaration>(parser->parse_expression());
	}

	std::unique_ptr<ASTNode> parse_call(Parser* parser)
	{
		auto name = parser->current()->value;

		parser->consume(TokenType::Id);
		parser->consume(TokenType::LParen);

		auto return_type = std::make_unique<TinyType>(Type::Unresolved);

		auto fn = parser->current_scope()->get_entry(name);
		if (fn == nullptr)
			parser->register_error("The function '" + name + "' has not been defined, Line: " + std::to_string(parser->current()->line_number));
		else
			return_type = std::make_unique<TinyType>(fn->value->type);

		auto exp = std::make_unique<CallExp>(std::move(return_type));

		while(parser->current()->type != TokenType::RParen)
		{
			exp->args.push_back(parser->parse_expression());
			
			if (parser->current()->type == TokenType::Comma)
				parser->consume(TokenType::Comma);
		}

		parser->consume(TokenType::RParen);

		return std::move(exp);
	}

}