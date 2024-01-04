#pragma once

#include <Engine/Common/Types.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class  ImageVariance
	{
	public:

		static Image* CreateImage2DR(void* Data, U32 Width, U32 Height, VkFormat Format = VK_FORMAT_R8_UNORM);
		static Image* CreateImage2DRG(void* Data, U32 Width, U32 Height, VkFormat Format = VK_FORMAT_R8G8_UNORM);
		static Image* CreateImage2DRGB(void* Data, U32 Width, U32 Height, VkFormat Format = VK_FORMAT_R8G8B8_UNORM);
		static Image* CreateImage2DRGBA(void* Data, U32 Width, U32 Height, VkFormat Format = VK_FORMAT_R8G8B8A8_UNORM);

		static Image* CreateDepthImage(U32 Width, U32 Height, VkFormat Format);
	};
}