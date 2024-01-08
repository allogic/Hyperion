#pragma once

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	struct FixedSizeBlock
	{
		U32 Allocated : 1;
		U32 Reserved : 31;
		FixedSizeAccessor* Accessor;
	};
}