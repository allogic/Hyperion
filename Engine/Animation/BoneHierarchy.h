#pragma once

#include <vector>
#include <string>

#include <Engine/Forward.h>

#include <Engine/Common/Config.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class BoneHierarchy
	{
	public:

		inline auto GetBuffer() const { return mBoneBuffer; }

	public:

		BoneHierarchy();
		virtual ~BoneHierarchy();

	public:

		FixedSizeAccessor* AllocateBone();

	private:

		void BuildDescriptorSet();

	public:

		void UpdateDescriptorSet(Animation* Animation);

	public:

		void Dispatch();

	private:

		Buffer* mBoneBuffer = 0;

		FixedSizeAllocator* mBoneAllocator = 0;

		std::string mComputeFile = ROOT_PATH "Engine\\Shaders\\BoneHierarchy.comp.spv";

		std::vector<VkDescriptorSetLayoutBinding> mDescriptorSetLayoutBindings =
		{
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 8, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
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