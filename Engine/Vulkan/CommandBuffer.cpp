#include <Engine/Common/Macros.h>

#include <Engine/Vulkan/CommandBuffer.h>
#include <Engine/Vulkan/Window.h>

namespace hyperion
{
	VkCommandBuffer CommandBuffer::Begin()
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandPool = gWindow->GetCommandPool();
		commandBufferAllocateInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer = {};
		VK_CHECK(vkAllocateCommandBuffers(gWindow->GetDevice(), &commandBufferAllocateInfo, &commandBuffer));

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		return commandBuffer;
	}

	void CommandBuffer::End(VkCommandBuffer CommandBuffer)
	{
		VK_CHECK(vkEndCommandBuffer(CommandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &CommandBuffer;

		VK_CHECK(vkQueueSubmit(gWindow->GetGraphicQueue(), 1, &submitInfo, 0));
		VK_CHECK(vkQueueWaitIdle(gWindow->GetGraphicQueue()));

		vkFreeCommandBuffers(gWindow->GetDevice(), gWindow->GetCommandPool(), 1, &CommandBuffer);
	}
}