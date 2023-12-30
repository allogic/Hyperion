#include <Common/Macros.h>

#include <Engine/Transform.h>
#include <Engine/TransformHierarchy.h>

#include <Vulkan/Pipeline.h>
#include <Vulkan/BufferVariance.h>
#include <Vulkan/Window.h>
#include <Vulkan/Buffer.h>
#include <Vulkan/Renderer.h>

namespace engine
{
	static std::vector<VkDescriptorSetLayoutBinding> sDescriptorSetLayoutBindings =
	{
		{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
		{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
		{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
		{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
	};

	static std::vector<VkPushConstantRange> sPushConstantRanges =
	{

	};

	TransformHierarchyLevel::TransformHierarchyLevel(Buffer* ParentBuffer, VkDescriptorSetLayout DescriptorSetLayout)
	{
		mDescriptorSetLayout = DescriptorSetLayout;

		mBuffer = BufferVariance::CreateStorageCoherent(sizeof(Transform) * 128);
		mParentBuffer = (ParentBuffer) ? ParentBuffer : mBuffer;

		mDescriptorPool = Pipeline::CreateDescriptorPool(1, sDescriptorSetLayoutBindings);

		BuildDescriptorSet();
		UpdateDescriptorSet();
	}

	TransformHierarchyLevel::~TransformHierarchyLevel()
	{
		vkDestroyDescriptorPool(gWindow->GetDevice(), mDescriptorPool, 0);

		delete mBuffer;
	}

	Transform* TransformHierarchyLevel::GetTransform(U32 TransformIndex)
	{
		return mBuffer->GetMappedData<Transform>() + TransformIndex;
	}

	void TransformHierarchyLevel::IncrementTransformCount()
	{
		mCount++;
	}

	void TransformHierarchyLevel::BuildDescriptorSet()
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

		VK_CHECK(vkAllocateDescriptorSets(gWindow->GetDevice(), &descriptorSetAllocateInfo, &mDescriptorSet));
	}

	void TransformHierarchyLevel::UpdateDescriptorSet()
	{
		VkDescriptorBufferInfo timeInfoDescriptorBufferInfo = {};
		timeInfoDescriptorBufferInfo.offset = 0;
		timeInfoDescriptorBufferInfo.buffer = gRenderer->GetTimeInfoBuffer()->GetBuffer();
		timeInfoDescriptorBufferInfo.range = gRenderer->GetTimeInfoBuffer()->GetSize();

		VkDescriptorBufferInfo viewProjectionDescriptorBufferInfo = {};
		viewProjectionDescriptorBufferInfo.offset = 0;
		viewProjectionDescriptorBufferInfo.buffer = gRenderer->GetViewProjectionBuffer()->GetBuffer();
		viewProjectionDescriptorBufferInfo.range = gRenderer->GetViewProjectionBuffer()->GetSize();

		VkDescriptorBufferInfo transformDescriptorBufferInfo = {};
		transformDescriptorBufferInfo.offset = 0;
		transformDescriptorBufferInfo.buffer = mBuffer->GetBuffer();
		transformDescriptorBufferInfo.range = mBuffer->GetSize();

		VkDescriptorBufferInfo parentTransformDescriptorBufferInfo = {};
		parentTransformDescriptorBufferInfo.offset = 0;
		parentTransformDescriptorBufferInfo.buffer = mParentBuffer->GetBuffer();
		parentTransformDescriptorBufferInfo.range = mParentBuffer->GetSize();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {};
		writeDescriptorSets.resize(sDescriptorSetLayoutBindings.size());

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].pNext = 0;
		writeDescriptorSets[0].dstSet = mDescriptorSet;
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pImageInfo = 0;
		writeDescriptorSets[0].pBufferInfo = &timeInfoDescriptorBufferInfo;
		writeDescriptorSets[0].pTexelBufferView = 0;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].pNext = 0;
		writeDescriptorSets[1].dstSet = mDescriptorSet;
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = 0;
		writeDescriptorSets[1].pBufferInfo = &viewProjectionDescriptorBufferInfo;
		writeDescriptorSets[1].pTexelBufferView = 0;

		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].pNext = 0;
		writeDescriptorSets[2].dstSet = mDescriptorSet;
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].dstArrayElement = 0;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].pImageInfo = 0;
		writeDescriptorSets[2].pBufferInfo = &transformDescriptorBufferInfo;
		writeDescriptorSets[2].pTexelBufferView = 0;

		writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[3].pNext = 0;
		writeDescriptorSets[3].dstSet = mDescriptorSet;
		writeDescriptorSets[3].dstBinding = 3;
		writeDescriptorSets[3].dstArrayElement = 0;
		writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[3].descriptorCount = 1;
		writeDescriptorSets[3].pImageInfo = 0;
		writeDescriptorSets[3].pBufferInfo = &parentTransformDescriptorBufferInfo;
		writeDescriptorSets[3].pTexelBufferView = 0;

		vkUpdateDescriptorSets(gWindow->GetDevice(), (U32)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, 0);
	}

	TransformHierarchy::TransformHierarchy()
	{
		mDescriptorSetLayout = Pipeline::CreateDescriptorSetLayout(sDescriptorSetLayoutBindings);
		mPipelineLayout = Pipeline::CreatePipelineLayout(mDescriptorSetLayout, sPushConstantRanges);
		mPipeline = Pipeline::CreateComputePipeline(mPipelineLayout, mComputeFile);
	}

	TransformHierarchy::~TransformHierarchy()
	{
		for (auto const& level : mLevels)
		{
			delete level;
		}

		vkDestroyDescriptorSetLayout(gWindow->GetDevice(), mDescriptorSetLayout, 0);
		vkDestroyPipelineLayout(gWindow->GetDevice(), mPipelineLayout, 0);
		vkDestroyPipeline(gWindow->GetDevice(), mPipeline, 0);
	}

	U32 TransformHierarchy::GetNextTransformIndex(U32 LevelIndex)
	{
		return mLevels[LevelIndex]->GetCount();
	}

	Transform* TransformHierarchy::GetTransform(U32 LevelIndex, U32 TransformIndex)
	{
		return mLevels[LevelIndex]->GetTransform(TransformIndex);
	}

	void TransformHierarchy::IncrementTransformCount(U32 LevelIndex)
	{
		mLevels[LevelIndex]->IncrementTransformCount();
	}

	void TransformHierarchy::Update(U32 LevelIndex)
	{
		if (LevelIndex >= mLevels.size())
		{
			mLevels.resize(LevelIndex + 1);

			Buffer* parentBuffer = (LevelIndex > 0) ? mLevels[LevelIndex - 1]->GetBuffer() : 0;

			mLevels[LevelIndex] = new TransformHierarchyLevel{ parentBuffer, mDescriptorSetLayout };
		}
	}

	void TransformHierarchy::Dispatch()
	{
		for (auto const& level : mLevels)
		{
			vkCmdBindPipeline(gRenderer->GetComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline);

			vkCmdBindDescriptorSets(gRenderer->GetComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipelineLayout, 0, 1, level->GetDescriptorSet(), 0, 0);

			//vkCmdPushConstants(gRenderer->GetComputeCommandBuffer(), mTransformPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PerEntityData), &perEntityData);

			vkCmdDispatch(gRenderer->GetComputeCommandBuffer(), 4, 1, 1);
		}
	}

	void TransformHierarchy::Print()
	{
		LOG("Transform hierarchy:\n");

		for (U32 i = 0; i < (U32)mLevels.size(); ++i)
		{
			LOG("  Level %u Size %u Entities %u\n", i, 32, mLevels[i]->GetCount());
		}

		LOG("\n");
	}
}