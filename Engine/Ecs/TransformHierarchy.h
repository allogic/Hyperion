#pragma once

#include <vector>
#include <string>

#include <Engine/Forward.h>

#include <Engine/Common/Config.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class TransformHierarchy
	{
	public:

		inline auto GetTransformBuffer() const { return mTransformBuffer; }

	public:

		TransformHierarchy();
		virtual ~TransformHierarchy();

	public:

		FixedSizeAccessor* AllocateTransform();

	private:

		void BuildDescriptorSet();
		void UpdateDescriptorSet();

	public:

		void Dispatch();

	private:

		Buffer* mTransformBuffer = 0;

		FixedSizeAllocator* mTransformAllocator = 0;

		std::string mComputeFile = ROOT_PATH "Engine\\Shaders\\TransformHierarchy.comp.spv";
		
		std::vector<VkDescriptorSetLayoutBinding> mDescriptorSetLayoutBindings =
		{
			{ 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
		};

		std::vector<VkPushConstantRange> mPushConstantRanges =
		{

		};

		VkDescriptorPool mDescriptorPool = 0;
		VkDescriptorSetLayout mDescriptorSetLayout = 0;

		VkDescriptorSet mDescriptorSet = 0;

		VkPipelineLayout mPipelineLayout = 0;
		VkPipeline mPipeline = 0;
	};
}