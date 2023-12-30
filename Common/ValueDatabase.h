#pragma once

#include <string>
#include <map>

#include <Common/Types.h>

namespace common
{
	class ValueDatabase
	{
	public:

	public:

		static R32 GetValue(std::string const& Name);

	public:

		static void SetValue(std::string const& Name, R32 Value);
	};
}