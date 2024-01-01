#pragma once

#include <Engine/Common/Types.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class CommandBuffer
	{
	public:

		static VkCommandBuffer Begin();
		static void End(VkCommandBuffer CommandBuffer);
	};
}