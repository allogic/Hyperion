#pragma once

#include <Engine/Common/Types.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class  ImageVariance
	{
	public:

		static Image* CreateRImage2D(void* Data, U32 Width, U32 Height);
		static Image* CreateRgImage2D(void* Data, U32 Width, U32 Height);
		static Image* CreateRgbImage2D(void* Data, U32 Width, U32 Height);
		static Image* CreateRgbaImage2D(void* Data, U32 Width, U32 Height);

		static Image* CreateDepthImage(U32 Width, U32 Height, VkFormat Format);
	};
}