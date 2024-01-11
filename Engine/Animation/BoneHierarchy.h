#pragma once

#include <vector>
#include <string>

#include <Engine/Forward.h>

#include <Engine/Animation/AnimatorState.h>

#include <Engine/Common/Config.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class BoneHierarchy
	{
	public:

		inline auto GetBoneBuffer() const { return mBoneBuffer; }

	public:

		BoneHierarchy();
		virtual ~BoneHierarchy();

	public:

		FixedSizeAccessor* AllocateBone();

	public:

		U32 AddDescriptorSet();
		void UpdateDescriptorSet(U32 DescriptorIndex, AnimatorComponent* AnimatorComponent, Animation* Animation);

	public:

		Buffer* CopyBoneBuffer();

	public:

		void Dispatch(U32 DescriptorIndex, AnimatorComponent* AnimatorComponent);

	private:

		Buffer* mBoneBuffer = 0;

		FixedSizeAllocator* mBoneAllocator = 0;

		std::string mComputeFile = ROOT_PATH "Engine\\Shaders\\BoneHierarchy.comp.spv";

		std::vector<VkDescriptorSetLayoutBinding> mDescriptorSetLayoutBindings =
		{
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
		};

		std::vector<VkPushConstantRange> mPushConstantRanges =
		{
			{ VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(AnimatorState) },
		};

		VkDescriptorPool mDescriptorPool = 0;
		VkDescriptorSetLayout mDescriptorSetLayout = 0;

		std::vector<VkDescriptorSet> mDescriptorSets = {};

		VkPipelineLayout mPipelineLayout = 0;
		VkPipeline mPipeline = 0;
	};
}