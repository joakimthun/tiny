#include "type.h"
#include "tiny_exception.h"
#include "token.h"

namespace tiny {

	std::string get_type_name(Type t)
	{
		switch (t)
		{
		case Type::Undefined:
			return "undefined";
		case Type::Void: 
			return "void";
		case Type::UserDefined: 
			throw "get_type_name -> UserDefined";
		case Type::Fn: 
			return "fn";
		case Type::I32: 
			return "i32";
		default: 
			throw TinyException("get_type_name -> default case");
		}
	}

	std::unique_ptr<TinyType> get_type_from_token(TokenType t)
	{
		switch (t)
		{
		case TokenType::Fn:
			return std::make_unique<TinyType>(Type::Fn);
		case TokenType::IntLiteral:
		case TokenType::I32:
			return std::make_unique<TinyType>(Type::I32);
		default:
			throw TinyException("get_type_from_token -> default case");
		}
	}
}
