#pragma once

#include <Common/Types.h>

#include <Vulkan/Forward.h>

#include <Vendor/vulkan/vulkan.h>

namespace vulkan
{
	class CommandBuffer
	{
	public:

		static VkCommandBuffer Begin();
		static void End(VkCommandBuffer CommandBuffer);
	};
}