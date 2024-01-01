#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/CommandBuffer.h>
#include <Engine/Vulkan/ImageVariance.h>

namespace hyperion
{
	Image* ImageVariance::CreateRImage2D(U32 Width, U32 Height, void* Data)
	{
		Buffer stagingBuffer = { Width * Height * 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data);
		stagingBuffer.Unmap();

		Image* targetImage = new Image{ Width, Height, 1, 1, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_NEAREST };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stagingBuffer.CopyToImage(commandBuffer, targetImage, Width, Height, 1, VK_IMAGE_ASPECT_COLOR_BIT);
		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

		CommandBuffer::End(commandBuffer);

		return targetImage;
	}

	Image* ImageVariance::CreateRgImage2D(U32 Width, U32 Height, void* Data)
	{
		Buffer stagingBuffer = { Width * Height * 2, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data);
		stagingBuffer.Unmap();

		Image* targetImage = new Image{ Width, Height, 1, 2, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_R8G8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_NEAREST };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stagingBuffer.CopyToImage(commandBuffer, targetImage, Width, Height, 1, VK_IMAGE_ASPECT_COLOR_BIT);
		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

		CommandBuffer::End(commandBuffer);

		return targetImage;
	}

	Image* ImageVariance::CreateRgbImage2D(U32 Width, U32 Height, void* Data)
	{
		Buffer stagingBuffer = { Width * Height * 3, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data);
		stagingBuffer.Unmap();

		Image* targetImage = new Image{ Width, Height, 1, 3, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_B8G8R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_NEAREST };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stagingBuffer.CopyToImage(commandBuffer, targetImage, Width, Height, 1, VK_IMAGE_ASPECT_COLOR_BIT);
		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

		CommandBuffer::End(commandBuffer);

		return targetImage;
	}

	Image* ImageVariance::CreateRgbaImage2D(U32 Width, U32 Height, void* Data)
	{
		Buffer stagingBuffer = { Width * Height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.Map();
		stagingBuffer.SetMappedData(Data);
		stagingBuffer.Unmap();

		Image* targetImage = new Image{ Width, Height, 1, 4, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_NEAREST };

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();

		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		stagingBuffer.CopyToImage(commandBuffer, targetImage, Width, Height, 1, VK_IMAGE_ASPECT_COLOR_BIT);
		targetImage->LayoutTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

		CommandBuffer::End(commandBuffer);

		return targetImage;
	}

	Image* ImageVariance::CreateDepthImage(U32 Width, U32 Height, VkFormat Format)
	{
		return new Image{ Width, Height, 1, 1, VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, Format, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_NEAREST };
	}
}