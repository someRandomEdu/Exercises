#ifndef DEBUG_DEFINED
#define DEBUG_DEFINED

#pragma once

#include <iostream>
#include <functional>

namespace Lib::Debug
{
	struct LoggedStruct final
	{
	public:
		using FunctionType = void(*)();

		FunctionType constructor = []() { std::cout << "LoggedStruct::LoggedStruct()\n"; };
		FunctionType destructor = []() { std::cout << "LoggedStruct::~LoggedStruct()\n"; };
		FunctionType copyConstructor = []() { std::cout << "LoggedStruct::LoggedStruct(const LoggedStruct &)\n"; };
		FunctionType moveConstructor = []() { std::cout << "LoggedStruct::LoggedStruct(LoggedStruct &)\n"; };
		FunctionType copyAssignment = []() { std::cout << "LoggedStruct &LoggedStruct::operator=(const LoggedStruct &)\n"; };
		FunctionType moveAssignment = []() { std::cout << "LoggedStruct &LoggedStruct::operator=(LoggedStruct &&)\n"; };

	private:
		void CopyFrom(const LoggedStruct &other)
		{
			constructor = other.constructor;
			destructor = other.destructor;
			copyConstructor = other.copyConstructor;
			moveConstructor = other.moveConstructor;
			copyAssignment = other.copyAssignment;
			moveAssignment = other.moveAssignment;
		}

	public:
		LoggedStruct() noexcept
		{
			constructor();
		}

		LoggedStruct(const LoggedStruct &other) noexcept
		{
			CopyFrom(other);
			copyConstructor();
		}

		LoggedStruct(LoggedStruct &&other) noexcept
		{
			CopyFrom(other);
			copyConstructor();
		}

		LoggedStruct &operator=(const LoggedStruct &other) noexcept
		{
			CopyFrom(other);
			copyAssignment();
			return *this;
		}

		LoggedStruct &operator=(LoggedStruct &&other) noexcept
		{
			CopyFrom(other);
			copyAssignment();
			return *this;
		}

		~LoggedStruct()
		{
			destructor();
		}
	};
}

#endif // DEBUG_DEFINED
