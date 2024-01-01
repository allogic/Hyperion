#include <Engine/Common/ValueDatabase.h>

namespace hyperion
{
	static std::map<std::string, R32> sValues = {};

	R32 ValueDatabase::GetValue(std::string const& Name)
	{
		return sValues[Name];
	}

	void ValueDatabase::SetValue(std::string const& Name, R32 Value)
	{
		sValues[Name] = Value;
	}
}