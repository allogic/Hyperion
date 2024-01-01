#pragma once

#include <Engine/Common/Types.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class  ImageVariance
	{
	public:

		static Image* CreateRImage2D(U32 Width, U32 Height, void* Data);
		static Image* CreateRgImage2D(U32 Width, U32 Height, void* Data);
		static Image* CreateRgbImage2D(U32 Width, U32 Height, void* Data);
		static Image* CreateRgbaImage2D(U32 Width, U32 Height, void* Data);

		static Image* CreateDepthImage(U32 Width, U32 Height, VkFormat Format);
	};
}