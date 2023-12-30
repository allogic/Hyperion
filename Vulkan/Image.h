#pragma once

#include <string>

#include <Common/Types.h>

#include <Vulkan/Forward.h>

#include <Vendor/vulkan/vulkan.h>

namespace vulkan
{
	using namespace common;

	class Image
	{
	public:

		inline auto GetSize() const { return mSize; }
		inline auto GetWidth() const { return mWidth; }
		inline auto GetHeight() const { return mHeight; }
		inline auto GetFormat() const { return mFormat; }
		inline auto GetImage() const { return mImage; }
		inline auto GetImageView() const { return mImageView; }
		inline auto GetSampler() const { return mSampler; }
		inline auto GetDeviceMemory() const { return mDeviceMemory; }

		template<typename T>
		inline auto GetMappedData() const { return (T*)mMappedData; }

	public:

		template<typename T>
		inline void SetMappedData(T* Data, U64 Size = 0) const { std::memcpy(mMappedData, Data, (Size) ? Size : mSize); }

	public:

		Image(U32 Width, U32 Height, U32 Depth, U32 Channels, VkImageType Type, VkImageViewType ViewType, VkImageUsageFlags Usage, VkMemoryPropertyFlags MemoryProperties, VkImageAspectFlags AspectFlags, VkFormat Format, VkImageTiling Tiling, VkFilter Filter);
		virtual ~Image();

	public:

		void Map();
		void Unmap();

	public:

		void CopyToImage(VkCommandBuffer CommandBuffer, Image* Target);
		void LayoutTransition(VkCommandBuffer CommandBuffer, VkImageLayout OldLayout, VkImageLayout NewLayout);

	public:

		static VkImageView CreateImageView(VkImage Image, VkImageViewType ViewType, VkFormat Format, VkImageAspectFlags AspectFlags);
		static VkSampler CreateSampler(VkFilter Filter);

		static void DestroyImageView(VkImageView ImageView);
		static void DestroySampler(VkSampler Sampler);

	private:

		U64 mSize = 0;

		U32 mWidth = 0;
		U32 mHeight = 0;
		U32 mDepth = 0;

		U32 mChannels = 0;

		VkImageType mType = VK_IMAGE_TYPE_1D;
		VkImageViewType mViewType = VK_IMAGE_VIEW_TYPE_1D;

		VkFormat mFormat = VK_FORMAT_UNDEFINED;
		VkImageTiling mTiling = VK_IMAGE_TILING_OPTIMAL;
		VkFilter mFilter = VK_FILTER_NEAREST;

		VkImage mImage = 0;
		VkImageView mImageView = 0;
		VkSampler mSampler = 0;

		VkDeviceMemory mDeviceMemory = 0;

		void* mMappedData = 0;
	};
}