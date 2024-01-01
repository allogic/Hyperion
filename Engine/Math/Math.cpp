#include <Engine/Math/Math.h>

namespace hyperion
{
	R32 Math::Remap(R32 Value, R32 FromLow, R32 FromHigh, R32 ToLow, R32 ToHigh)
	{
		return ToLow + (Value - FromLow) * (ToHigh - ToLow) / (FromHigh - FromLow);
	}

	R32 Math::Lerp(R32 From, R32 To, R32 Time)
	{
		return From + Time * (To - From);
	}
}