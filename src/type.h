#pragma once

#include <stdint.h>
#include <string>

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

namespace tiny {

	enum class Type : u16
	{
		Void,
		UserDefined,
		Fn,
		I32,
	};

	inline std::string get_type_name(Type t)
	{
		switch (t)
		{
		case Type::Void: return "void";
		case Type::UserDefined: throw "get_type_name -> UserDefined";
		case Type::Fn: return "fn";
		case Type::I32: return "i32";
		default: throw "get_type_name";
		}
	}

	struct TinyType
	{
		TinyType(Type t) : type(t), name(get_type_name(t)) {}

		Type type;
		std::string name;

		bool operator==(const TinyType& other) const
		{
			return this->type == other.type && this->name == other.name;
		}

		bool are_equal(const TinyType* other) const
		{
			return *this == *other;
		}
	};

}