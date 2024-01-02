#pragma once

#include <Engine/Common/Types.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	constexpr U32 TextVertexBindingId = 0;
	constexpr U32 InterfaceVertexBindingId = 0;
	constexpr U32 DebugVertexBindingId = 0;

#pragma pack (push, 1)
	struct PbrVertex
	{
		R32V4 Position;
		R32V4 Normal;
		R32V4 TexCoord0;
		R32V4 Weight0;
		R32V4 Joint0;

	};

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