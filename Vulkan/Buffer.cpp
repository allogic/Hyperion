#include <Common/Macros.h>

#include <Vulkan/Buffer.h>
#include <Vulkan/Window.h>
#include <Vulkan/Image.h>

namespace vulkan
{
	Buffer::Buffer(U64 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags MemoryProperties)
	{
		mSize = Size;

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = Size;
		bufferCreateInfo.usage = Usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK(vkCreateBuffer(gWindow->GetDevice(), &bufferCreateInfo, 0, &mBuffer));

		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(gWindow->GetDevice(), mBuffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = gWindow->CheckMemoryType(memoryRequirements.memoryTypeBits, MemoryProperties);

		VK_CHECK(vkAllocateMemory(gWindow->GetDevice(), &memoryAllocateInfo, 0, &mDeviceMemory));
		VK_CHECK(vkBindBufferMemory(gWindow->GetDevice(), mBuffer, mDeviceMemory, 0));
	}

	Buffer::~Buffer()
	{
		if (mMappedData)
		{
			vkUnmapMemory(gWindow->GetDevice(), mDeviceMemory);
		}

		vkFreeMemory(gWindow->GetDevice(), mDeviceMemory, 0);

		vkDestroyBuffer(gWindow->GetDevice(), mBuffer, 0);
	}

	void Buffer::Map()
	{
		VK_CHECK(vkMapMemory(gWindow->GetDevice(), mDeviceMemory, 0, mSize, 0, &mMappedData));
	}

	void Buffer::Unmap()
	{
		vkUnmapMemory(gWindow->GetDevice(), mDeviceMemory);

		mMappedData = 0;
	}

	void Buffer::CopyToBuffer(VkCommandBuffer CommandBuffer, Buffer* Target, U64 Size)
	{
		VkBufferCopy bufferCopy = {};
		bufferCopy.size = Size;

		vkCmdCopyBuffer(CommandBuffer, mBuffer, Target->mBuffer, 1, &bufferCopy);
	}

	void Buffer::CopyToImage(VkCommandBuffer CommandBuffer, Image* Target, U32 Width, U32 Height, U32 Depth, VkImageAspectFlags AspectFlags)
	{
		VkBufferImageCopy bufferImageCopy = {};
		bufferImageCopy.bufferOffset = 0;
		bufferImageCopy.bufferRowLength = 0;
		bufferImageCopy.bufferImageHeight = 0;
		bufferImageCopy.imageSubresource.aspectMask = AspectFlags;
		bufferImageCopy.imageSubresource.mipLevel = 0;
		bufferImageCopy.imageSubresource.baseArrayLayer = 0;
		bufferImageCopy.imageSubresource.layerCount = 1;
		bufferImageCopy.imageOffset.x = 0;
		bufferImageCopy.imageOffset.y = 0;
		bufferImageCopy.imageOffset.z = 0;
		bufferImageCopy.imageExtent.width = Width;
		bufferImageCopy.imageExtent.height = Height;
		bufferImageCopy.imageExtent.depth = Depth;

		vkCmdCopyBufferToImage(CommandBuffer, mBuffer, Target->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);
	}
}