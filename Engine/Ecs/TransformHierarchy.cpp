#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Transform.h>
#include <Engine/Ecs/TransformHierarchy.h>

#include <Engine/Memory/FixedSizeBlock.h>
#include <Engine/Memory/FixedSizeAccessor.h>
#include <Engine/Memory/FixedSizeAllocator.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Renderer.h>

#include <Engine/Vulkan/Pipeline.h>
#include <Engine/Vulkan/BufferVariance.h>
#include <Engine/Vulkan/Buffer.h>

namespace hyperion
{
	TransformHierarchy::TransformHierarchy()
	{
		mTransformBuffer = BufferVariance::CreateStorageCoherent(sizeof(FixedSizeBlock) * sizeof(Transform) * 1024);
		mTransformAllocator = new FixedSizeAllocator{ mTransformBuffer->GetMappedData<U8>(), 1024, sizeof(Transform) };

		mDescriptorPool = Pipeline::CreateDescriptorPool(1, mDescriptorSetLayoutBindings);
		mDescriptorSetLayout = Pipeline::CreateDescriptorSetLayout(mDescriptorSetLayoutBindings);
		mPipelineLayout = Pipeline::CreatePipelineLayout(mDescriptorSetLayout, mPushConstantRanges);
		mPipeline = Pipeline::CreateComputePipeline(mPipelineLayout, mComputeFile);

		BuildDescriptorSet();
		UpdateDescriptorSet();
	}

	TransformHierarchy::~TransformHierarchy()
	{
		vkDestroyPipeline(gWindow->GetDevice(), mPipeline, 0);
		vkDestroyPipelineLayout(gWindow->GetDevice(), mPipelineLayout, 0);
		vkDestroyDescriptorSetLayout(gWindow->GetDevice(), mDescriptorSetLayout, 0);
		vkDestroyDescriptorPool(gWindow->GetDevice(), mDescriptorPool, 0);

		delete mTransformAllocator;
		delete mTransformBuffer;
	}

	FixedSizeAccessor* TransformHierarchy::AllocateTransform()
	{
		return mTransformAllocator->Allocate();
	}

	void TransformHierarchy::BuildDescriptorSet()
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

		VK_CHECK(vkAllocateDescriptorSets(gWindow->GetDevice(), &descriptorSetAllocateInfo, &mDescriptorSet));
	}

	void TransformHierarchy::UpdateDescriptorSet()
	{
		VkDescriptorBufferInfo timeInfoDescriptorBufferInfo = {};
		timeInfoDescriptorBufferInfo.offset = 0;
		timeInfoDescriptorBufferInfo.buffer = gRenderer->GetTimeInfoBuffer()->GetBuffer();
		timeInfoDescriptorBufferInfo.range = gRenderer->GetTimeInfoBuffer()->GetSize();

		VkDescriptorBufferInfo projectionInfoDescriptorBufferInfo = {};
		projectionInfoDescriptorBufferInfo.offset = 0;
		projectionInfoDescriptorBufferInfo.buffer = gRenderer->GetProjectionInfoBuffer()->GetBuffer();
		projectionInfoDescriptorBufferInfo.range = gRenderer->GetProjectionInfoBuffer()->GetSize();

		VkDescriptorBufferInfo transformDescriptorBufferInfo = {};
		transformDescriptorBufferInfo.offset = 0;
		transformDescriptorBufferInfo.buffer = mTransformBuffer->GetBuffer();
		transformDescriptorBufferInfo.range = mTransformBuffer->GetSize();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {};
		writeDescriptorSets.resize(mDescriptorSetLayoutBindings.size());

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
		writeDescriptorSets[1].pBufferInfo = &projectionInfoDescriptorBufferInfo;
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

		vkUpdateDescriptorSets(gWindow->GetDevice(), (U32)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, 0);
	}

	void TransformHierarchy::Dispatch()
	{
		vkCmdBindPipeline(gRenderer->GetComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline);

		vkCmdBindDescriptorSets(gRenderer->GetComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipelineLayout, 0, 1, &mDescriptorSet, 0, 0);

		//vkCmdPushConstants(gRenderer->GetComputeCommandBuffer(), mTransformPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PerEntityData), &perEntityData);

		vkCmdDispatch(gRenderer->GetComputeCommandBuffer(), 1024 / 32, 1, 1);
	}

	void TransformHierarchy::Print()
	{
		LOG("Transform hierarchy\n");

		//for (U32 i = 0; i < (U32)mLevels.size(); ++i)
		//{
		//	LOG("  Level %u Size %u Entities %u\n", i, 32, mLevels[i]->GetCount());
		//}

		LOG("\n");
	}
}