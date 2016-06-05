#pragma once

#include <stdint.h>
#include <string>
#include <memory>

#include "tiny_exception.h"

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

namespace tiny {

	struct TinyType;
	enum class TokenType : u16;

	enum class Type : u16
	{
		Unresolved,
		Void,
		UserDefined,
		Fn,
		I32,
		I32Ptr,
		I8,
		I8Ptr,
	};

	std::string get_type_name(Type t);
	std::unique_ptr<TinyType> get_type_from_token(TokenType t, bool pointer);

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
			if (other == nullptr)
				throw TinyException("TinyType -> are_equal nullptr");

			return *this == *other;
		}
	};

}