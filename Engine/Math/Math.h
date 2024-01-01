#pragma once

#include <Engine/Common/Types.h>

namespace hyperion
{
	class Math
	{
	public:

		static R32 Remap(R32 Value, R32 FromLow, R32 FromHigh, R32 ToLow, R32 ToHigh);
		static R32 Lerp(R32 From, R32 To, R32 Time);
	};
}