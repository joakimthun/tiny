#pragma once

#include <exception>
#include <string>
#include <stdio.h>
#include <cstdarg>

namespace tiny
{
	class TinyException : public std::exception
	{
	public:
		TinyException::TinyException(const std::string& message) : message_(message) {}

		TinyException::TinyException(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);

			char b[1024];
			sprintf_s(b, 1024, fmt, args);
			message_ = std::string(b);
		}

		const char* TinyException::what() const throw() override
		{
			return message_.c_str();
		}

	private:
		std::string message_;
	};
}
