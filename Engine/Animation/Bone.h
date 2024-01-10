#pragma once

#include <Engine/Common/Types.h>

namespace hyperion
{
	struct Bone
	{
		U32 ChannelViewIndex;
		U32 Allocated;
		I32 ParentIndex;
		U32 Reserved0;
		R32M4 ParentTransform;
		R32M4 OffsetTransform;
		R32M4 LocalTransform;
		R32M4 WorldTransform;
	};
}