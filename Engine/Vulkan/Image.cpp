#include <Engine/Common/Macros.h>

#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/Window.h>

namespace hyperion
{
	Image::Image(U32 Width, U32 Height, U32 Depth, U32 Channels, VkImageType Type, VkImageViewType ViewType, VkImageUsageFlags Usage, VkMemoryPropertyFlags MemoryProperties, VkImageAspectFlags AspectFlags, VkFormat Format, VkImageTiling Tiling, VkFilter Filter)
	{
		mSize = Width * Height * Depth * Channels;
		mWidth = Width;
		mHeight = Height;
		mDepth = Depth;
		mChannels = Channels;
		mType = Type;
		mViewType = ViewType;
		mFormat = Format;
		mTiling = Tiling;
		mFilter = Filter;

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = mType;
		imageCreateInfo.extent.width = mWidth;
		imageCreateInfo.extent.height = mHeight;
		imageCreateInfo.extent.depth = mDepth;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = mFormat;
		imageCreateInfo.tiling = mTiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = Usage;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK(vkCreateImage(gWindow->GetDevice(), &imageCreateInfo, 0, &mImage));

		VkMemoryRequirements memoryRequirements = {};
		vkGetImageMemoryRequirements(gWindow->GetDevice(), mImage, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = gWindow->CheckMemoryType(memoryRequirements.memoryTypeBits, MemoryProperties);

		VK_CHECK(vkAllocateMemory(gWindow->GetDevice(), &memoryAllocateInfo, 0, &mDeviceMemory));
		VK_CHECK(vkBindImageMemory(gWindow->GetDevice(), mImage, mDeviceMemory, 0));

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = mImage;
		imageViewCreateInfo.viewType = mViewType;
		imageViewCreateInfo.format = mFormat;
		imageViewCreateInfo.subresourceRange.aspectMask = AspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(gWindow->GetDevice(), &imageViewCreateInfo, 0, &mImageView));

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = Filter;
		samplerCreateInfo.minFilter = Filter;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = gWindow->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0F;
		samplerCreateInfo.minLod = 0.0F;
		samplerCreateInfo.maxLod = 0.0F;

		VK_CHECK(vkCreateSampler(gWindow->GetDevice(), &samplerCreateInfo, 0, &mSampler));
	}

	Image::~Image()
	{
		if (mMappedData)
		{
			vkUnmapMemory(gWindow->GetDevice(), mDeviceMemory);
		}

		vkFreeMemory(gWindow->GetDevice(), mDeviceMemory, 0);

		vkDestroySampler(gWindow->GetDevice(), mSampler, 0);
		vkDestroyImageView(gWindow->GetDevice(), mImageView, 0);
		vkDestroyImage(gWindow->GetDevice(), mImage, 0);
	}

	void Image::Map()
	{
		VK_CHECK(vkMapMemory(gWindow->GetDevice(), mDeviceMemory, 0, mSize, 0, &mMappedData));
	}

	void Image::Unmap()
	{
		vkUnmapMemory(gWindow->GetDevice(), mDeviceMemory);

		mMappedData = 0;
	}

	void Image::CopyToImage(VkCommandBuffer CommandBuffer, Image* Target)
	{
		VkImageCopy imageCopy = {};
		imageCopy.extent.width = mWidth;
		imageCopy.extent.height = mWidth;
		imageCopy.extent.depth = mDepth;

		vkCmdCopyImage(CommandBuffer, mImage, VK_IMAGE_LAYOUT_UNDEFINED, Target->mImage, VK_IMAGE_LAYOUT_UNDEFINED, 1, &imageCopy);
	}

	void Image::LayoutTransition(VkCommandBuffer CommandBuffer, VkImageLayout OldLayout, VkImageLayout NewLayout)
	{
		VkImageMemoryBarrier imageMemoryBarrier = {};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.oldLayout = OldLayout;
		imageMemoryBarrier.newLayout = NewLayout;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = mImage;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = 1;
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = 0;

		VkPipelineStageFlags pipelineSourceStageFlags = VK_PIPELINE_STAGE_NONE;
		VkPipelineStageFlags pipelineDestinationStageFlags = VK_PIPELINE_STAGE_NONE;

		if ((OldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
		{
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (NewLayout == VK_IMAGE_LAYOUT_UNDEFINED))
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = 0;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (NewLayout == VK_IMAGE_LAYOUT_GENERAL))
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = 0;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) && (NewLayout == VK_IMAGE_LAYOUT_UNDEFINED))
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			imageMemoryBarrier.dstAccessMask = 0;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
		{
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_GENERAL) && (NewLayout == VK_IMAGE_LAYOUT_UNDEFINED))
		{
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = 0;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (NewLayout == VK_IMAGE_LAYOUT_GENERAL))
		{
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = 0;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_GENERAL) && (NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL))
		{
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if ((OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) && (NewLayout == VK_IMAGE_LAYOUT_GENERAL))
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			imageMemoryBarrier.dstAccessMask = 0;

			pipelineSourceStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			pipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}

		vkCmdPipelineBarrier(CommandBuffer, pipelineSourceStageFlags, pipelineDestinationStageFlags, 0, 0, 0, 0, 0, 1, &imageMemoryBarrier);
	}

	VkImageView Image::CreateImageView(VkImage Image, VkImageViewType ViewType, VkFormat Format, VkImageAspectFlags AspectFlags)
	{
		VkImageView imageView = 0;

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = Image;
		imageViewCreateInfo.viewType = ViewType;
		imageViewCreateInfo.format = Format;
		imageViewCreateInfo.subresourceRange.aspectMask = AspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(gWindow->GetDevice(), &imageViewCreateInfo, 0, &imageView));

		return imageView;
	}

	VkSampler Image::CreateSampler(VkFilter Filter)
	{
		VkSampler sampler = 0;

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = Filter;
		samplerCreateInfo.minFilter = Filter;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = gWindow->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0F;
		samplerCreateInfo.minLod = 0.0F;
		samplerCreateInfo.maxLod = 0.0F;

		VK_CHECK(vkCreateSampler(gWindow->GetDevice(), &samplerCreateInfo, 0, &sampler));

		return sampler;
	}

	void Image::DestroyImageView(VkImageView ImageView)
	{
		vkDestroyImageView(gWindow->GetDevice(), ImageView, 0);
	}

	void Image::DestroySampler(VkSampler Sampler)
	{
		vkDestroySampler(gWindow->GetDevice(), Sampler, 0);
	}
}