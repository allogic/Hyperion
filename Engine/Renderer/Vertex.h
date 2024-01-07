#pragma once

#include <Engine/Common/Types.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	constexpr U32 PhysicallyBasedVertexBindingId = 0;
	constexpr U32 TextVertexBindingId = 0;
	constexpr U32 InterfaceVertexBindingId = 0;
	constexpr U32 DebugVertexBindingId = 0;

#pragma pack (push, 1)
	struct PhysicallyBasedVertex
	{
		R32V3 Position;
		R32V3 Normal;
		R32V3 Tangent;
		R32V3 BiTangent;
		R32V4 ColorChannel0;
		R32V4 ColorChannel1;
		R32V4 ColorChannel2;
		R32V4 ColorChannel3;
		R32V4 ColorChannel4;
		R32V4 ColorChannel5;
		R32V4 ColorChannel6;
		R32V4 ColorChannel7;
		R32V3 TexCoordChannel0;
		R32V3 TexCoordChannel1;
		R32V3 TexCoordChannel2;
		R32V3 TexCoordChannel3;
		R32V3 TexCoordChannel4;
		R32V3 TexCoordChannel5;
		R32V3 TexCoordChannel6;
		R32V3 TexCoordChannel7;
		I32V4 BoneIds;
		R32V4 BoneWeights;
	};

	struct TextVertex
	{
		R32V3 Position;
		U32 ColorChannel0;
		R32V2 TexCoordChannel0;
	};

	struct InterfaceVertex
	{
		R32V3 Position;
		U32 ColorChannel0;
		R32V2 TexCoordChannel0;
	};

	struct DebugVertex
	{
		R32V3 Position;
		U32 ColorChannel0;
	};
#pragma pack (pop)
}