#pragma once

#include <vector>
#include <string>

#include <Engine/Common/Config.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class TransformHierarchyLevel
	{
	public:

		inline auto GetDescriptorSet() const { return &mDescriptorSet; }
		inline auto GetCount() const { return mCount; }
		inline auto GetBuffer() const { return mBuffer; }

	public:

		TransformHierarchyLevel(Buffer* ParentBuffer, VkDescriptorSetLayout DescriptorSetLayout);
		virtual ~TransformHierarchyLevel();

	public:

		Transform* GetTransform(U32 TransformIndex);

	public:

		void IncrementTransformCount();
		void BuildDescriptorSet();
		void UpdateDescriptorSet();

	private:

		Buffer* mBuffer = 0;
		Buffer* mParentBuffer = 0;

		VkDescriptorPool mDescriptorPool = 0;
		VkDescriptorSetLayout mDescriptorSetLayout = 0;
		VkDescriptorSet mDescriptorSet = 0;

		U32 mCount = 0;
	};

	class TransformHierarchy
	{
	public:

		TransformHierarchy();
		virtual ~TransformHierarchy();

	public:

		U32 GetNextTransformIndex(U32 LevelIndex);
		Transform* GetTransform(U32 LevelIndex, U32 TransformIndex);

	public:

		void IncrementTransformCount(U32 LevelIndex);
		void Update(U32 LevelIndex);
		void Dispatch();
		void Print();

	private:

		std::vector<TransformHierarchyLevel*> mLevels = {};

	private:

		std::string mComputeFile = ROOT_PATH "Engine\\Shaders\\Transform.comp.spv";

		VkDescriptorSetLayout mDescriptorSetLayout = 0;

		VkPipelineLayout mPipelineLayout = 0;
		VkPipeline mPipeline = 0;
	};
}
