#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/CommandBuffer.h>
#include <Engine/Vulkan/BufferVariance.h>

namespace hyperion
{
	Buffer* BufferVariance::CreateVertex(void* Data, U64 Size)
	{
		Buffer stagingBuffer = { Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data, Size);
		stagingBuffer.Unmap();

		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		stagingBuffer.CopyToBuffer(commandBuffer, targetBuffer, Size);

		CommandBuffer::End(commandBuffer);

		return targetBuffer;
	}

	Buffer* BufferVariance::CreateIndex(void* Data, U64 Size)
	{
		Buffer stagingBuffer = { Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data, Size);
		stagingBuffer.Unmap();

		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		stagingBuffer.CopyToBuffer(commandBuffer, targetBuffer, Size);

		CommandBuffer::End(commandBuffer);

		return targetBuffer;
	}

	Buffer* BufferVariance::CreateUniform(void* Data, U64 Size)
	{
		Buffer stagingBuffer = { Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data, Size);
		stagingBuffer.Unmap();

		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		stagingBuffer.CopyToBuffer(commandBuffer, targetBuffer, Size);

		CommandBuffer::End(commandBuffer);

		return targetBuffer;
	}

	Buffer* BufferVariance::CreateStorage(void* Data, U64 Size)
	{
		Buffer stagingBuffer = { Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data, Size);
		stagingBuffer.Unmap();

		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		stagingBuffer.CopyToBuffer(commandBuffer, targetBuffer, Size);

		CommandBuffer::End(commandBuffer);

		return targetBuffer;
	}

	Buffer* BufferVariance::CreateVertexCoherent(U64 Size)
	{
		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		targetBuffer->Map();

		return targetBuffer;
	}

	Buffer* BufferVariance::CreateIndexCoherent(U64 Size)
	{
		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		targetBuffer->Map();

		return targetBuffer;
	}

	Buffer* BufferVariance::CreateUniformCoherent(U64 Size)
	{
		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		targetBuffer->Map();

		return targetBuffer;
	}

	Buffer* BufferVariance::CreateStorageCoherent(U64 Size)
	{
		Buffer* targetBuffer = new Buffer{ Size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		targetBuffer->Map();

		return targetBuffer;
	}
}