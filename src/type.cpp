#include "type.h"
#include "tiny_exception.h"
#include "token.h"

namespace tiny {

	std::string get_type_name(Type t)
	{
		switch (t)
		{
		case Type::Unresolved:
			return "unresolved";
		case Type::Void: 
			return "void";
		case Type::UserDefined: 
			throw "get_type_name -> UserDefined";
		case Type::Fn: 
			return "fn";
		case Type::I32: 
			return "i32";
		case Type::I32Ptr:
			return "i32Ptr";
		case Type::I8:
			return "i8";
		case Type::I8Ptr:
			return "i8Ptr";
		case Type::StringLit:
			return "String literal";
		default: 
			throw TinyException("get_type_name -> default case");
		}
	}

	std::unique_ptr<TinyType> get_type_from_token(TokenType t, bool pointer)
	{
		switch (t)
		{
		case TokenType::Fn:
			return std::make_unique<TinyType>(Type::Fn);
		case TokenType::IntLiteral:
		case TokenType::I32:
			return std::make_unique<TinyType>(pointer ? Type::I32Ptr : Type::I32);
		case TokenType::I8:
			return std::make_unique<TinyType>(pointer ? Type::I8Ptr : Type::I8);
		case TokenType::StringLiteral:
			return std::make_unique<TinyType>(Type::StringLit);
		default:
			throw TinyException("get_type_from_token -> default case");
		}
	}
}
