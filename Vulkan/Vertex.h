#pragma once

#include <Common/Types.h>

namespace vulkan
{
	using namespace common;

	constexpr U32 TextVertexBindingId = 0;
	constexpr U32 InterfaceVertexBindingId = 0;
	constexpr U32 DebugVertexBindingId = 0;

#pragma pack (push, 1)
	struct TextVertex
	{
		R32V3 Position;
		R32V2 Uv;
		U32 Color;
	};

	struct InterfaceVertex
	{
		R32V3 Position;
		R32V2 Uv;
		U32 Color;
	};

	struct DebugVertex
	{
		R32V3 Position;
		U32 Color;
	};
#pragma pack (pop)
}