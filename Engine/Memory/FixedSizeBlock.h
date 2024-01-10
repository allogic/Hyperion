#pragma once

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	struct FixedSizeBlock
	{
		U32 Allocated;
		U32 Reserved0;
		FixedSizeAccessor* Accessor;
	};
}