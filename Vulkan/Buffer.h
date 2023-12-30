#pragma once

#include <string>

#include <Common/Types.h>

#include <Vulkan/Forward.h>

#include <Vendor/vulkan/vulkan.h>

namespace vulkan
{
	using namespace common;

	class Buffer
	{
	public:

		inline auto GetSize() const { return mSize; }
		inline auto GetBuffer() const { return mBuffer; }
		inline auto GetDeviceMemory() const { return mDeviceMemory; }

		template<typename T>
		inline auto GetMappedData() const { return (T*)mMappedData; }

	public:

		template<typename T>
		inline void SetMappedData(T* Data, U64 Size = 0) const { std::memcpy(mMappedData, Data, (Size) ? Size : mSize); }

	public:

		Buffer(U64 Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags MemoryProperties);
		virtual ~Buffer();

	public:

		void Map();
		void Unmap();

		void CopyToBuffer(VkCommandBuffer CommandBuffer, Buffer* Target, U64 Size);
		void CopyToImage(VkCommandBuffer CommandBuffer, Image* Target, U32 Width, U32 Height, U32 Depth, VkImageAspectFlags AspectFlags);

	private:

		U64 mSize = 0;

		VkBuffer mBuffer = 0;

		VkDeviceMemory mDeviceMemory = 0;

		void* mMappedData = 0;
	};
}