#include <Engine/Animation/Animation.h>
#include <Engine/Animation/AnimatorState.h>
#include <Engine/Animation/Bone.h>
#include <Engine/Animation/BoneHierarchy.h>

#include <Engine/Common/Config.h>
#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Components/AnimatorComponent.h>

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
	BoneHierarchy::BoneHierarchy()
	{
		mBoneBuffer = BufferVariance::CreateStorageCoherent(sizeof(FixedSizeBlock) * sizeof(Bone) * MAX_BONES);
		mBoneAllocator = new FixedSizeAllocator{ mBoneBuffer->GetMappedData<U8>(), MAX_BONES, sizeof(Bone) };

		mDescriptorPool = Pipeline::CreateDescriptorPool(ANIMATION_DESCRIPTOR_POOL_SIZE, mDescriptorSetLayoutBindings);
		mDescriptorSetLayout = Pipeline::CreateDescriptorSetLayout(mDescriptorSetLayoutBindings);
		mPipelineLayout = Pipeline::CreatePipelineLayout(mDescriptorSetLayout, mPushConstantRanges);
		mPipeline = Pipeline::CreateComputePipeline(mPipelineLayout, mComputeFile);
	}

	BoneHierarchy::~BoneHierarchy()
	{
		vkDestroyPipeline(gWindow->GetDevice(), mPipeline, 0);
		vkDestroyPipelineLayout(gWindow->GetDevice(), mPipelineLayout, 0);
		vkDestroyDescriptorSetLayout(gWindow->GetDevice(), mDescriptorSetLayout, 0);
		vkDestroyDescriptorPool(gWindow->GetDevice(), mDescriptorPool, 0);

		delete mBoneAllocator;
		delete mBoneBuffer;
	}

	FixedSizeAccessor* BoneHierarchy::AllocateBone()
	{
		return mBoneAllocator->Allocate();
	}

	U32 BoneHierarchy::AddDescriptorSet()
	{
		U32 descriptorIndex = (U32)mDescriptorSets.size();

		auto& descriptorSet = mDescriptorSets.emplace_back();

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = &mDescriptorSetLayout;

		VK_CHECK(vkAllocateDescriptorSets(gWindow->GetDevice(), &descriptorSetAllocateInfo, &descriptorSet));

		return descriptorIndex;
	}

	void BoneHierarchy::UpdateDescriptorSet(U32 DescriptorIndex, AnimatorComponent* AnimatorComponent, Animation* Animation)
	{
		Buffer* timeInfoBuffer = gRenderer->GetTimeInfoBuffer();

		Buffer* boneBuffer = AnimatorComponent->GetBoneBuffer();

		Buffer* positionBoneChannelViewBuffer = Animation->GetPositionBoneChannelViewBuffer();
		Buffer* rotationBoneChannelViewBuffer = Animation->GetRotationBoneChannelViewBuffer();
		Buffer* scaleBoneChannelViewBuffer = Animation->GetScaleBoneChannelViewBuffer();

		Buffer* positionKeyFrameBuffer = Animation->GetPositionKeyFrameBuffer();
		Buffer* rotationKeyFrameBuffer = Animation->GetRotationKeyFrameBuffer();
		Buffer* scaleKeyFrameBuffer = Animation->GetScaleKeyFrameBuffer();

		VkDescriptorBufferInfo timeInfoDescriptorBufferInfo = {};
		timeInfoDescriptorBufferInfo.offset = 0;
		timeInfoDescriptorBufferInfo.buffer = timeInfoBuffer->GetBuffer();
		timeInfoDescriptorBufferInfo.range = timeInfoBuffer->GetSize();

		VkDescriptorBufferInfo boneDescriptorBufferInfo = {};
		boneDescriptorBufferInfo.offset = 0;
		boneDescriptorBufferInfo.buffer = boneBuffer->GetBuffer();
		boneDescriptorBufferInfo.range = boneBuffer->GetSize();

		VkDescriptorBufferInfo positionBoneChannelDescriptorBufferInfo = {};
		positionBoneChannelDescriptorBufferInfo.offset = 0;
		positionBoneChannelDescriptorBufferInfo.buffer = positionBoneChannelViewBuffer->GetBuffer();
		positionBoneChannelDescriptorBufferInfo.range = positionBoneChannelViewBuffer->GetSize();

		VkDescriptorBufferInfo rotationBoneChannelDescriptorBufferInfo = {};
		rotationBoneChannelDescriptorBufferInfo.offset = 0;
		rotationBoneChannelDescriptorBufferInfo.buffer = rotationBoneChannelViewBuffer->GetBuffer();
		rotationBoneChannelDescriptorBufferInfo.range = rotationBoneChannelViewBuffer->GetSize();

		VkDescriptorBufferInfo scaleBoneChannelDescriptorBufferInfo = {};
		scaleBoneChannelDescriptorBufferInfo.offset = 0;
		scaleBoneChannelDescriptorBufferInfo.buffer = scaleBoneChannelViewBuffer->GetBuffer();
		scaleBoneChannelDescriptorBufferInfo.range = scaleBoneChannelViewBuffer->GetSize();

		VkDescriptorBufferInfo positionKeyFrameDescriptorBufferInfo = {};
		positionKeyFrameDescriptorBufferInfo.offset = 0;
		positionKeyFrameDescriptorBufferInfo.buffer = positionKeyFrameBuffer->GetBuffer();
		positionKeyFrameDescriptorBufferInfo.range = positionKeyFrameBuffer->GetSize();

		VkDescriptorBufferInfo rotationKeyFrameDescriptorBufferInfo = {};
		rotationKeyFrameDescriptorBufferInfo.offset = 0;
		rotationKeyFrameDescriptorBufferInfo.buffer = rotationKeyFrameBuffer->GetBuffer();
		rotationKeyFrameDescriptorBufferInfo.range = rotationKeyFrameBuffer->GetSize();

		VkDescriptorBufferInfo scaleKeyFrameDescriptorBufferInfo = {};
		scaleKeyFrameDescriptorBufferInfo.offset = 0;
		scaleKeyFrameDescriptorBufferInfo.buffer = scaleKeyFrameBuffer->GetBuffer();
		scaleKeyFrameDescriptorBufferInfo.range = scaleKeyFrameBuffer->GetSize();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {};
		writeDescriptorSets.resize(mDescriptorSetLayoutBindings.size());

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].pNext = 0;
		writeDescriptorSets[0].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pImageInfo = 0;
		writeDescriptorSets[0].pBufferInfo = &timeInfoDescriptorBufferInfo;
		writeDescriptorSets[0].pTexelBufferView = 0;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].pNext = 0;
		writeDescriptorSets[1].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = 0;
		writeDescriptorSets[1].pBufferInfo = &boneDescriptorBufferInfo;
		writeDescriptorSets[1].pTexelBufferView = 0;

		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].pNext = 0;
		writeDescriptorSets[2].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].dstArrayElement = 0;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].pImageInfo = 0;
		writeDescriptorSets[2].pBufferInfo = &positionBoneChannelDescriptorBufferInfo;
		writeDescriptorSets[2].pTexelBufferView = 0;

		writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[3].pNext = 0;
		writeDescriptorSets[3].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[3].dstBinding = 3;
		writeDescriptorSets[3].dstArrayElement = 0;
		writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[3].descriptorCount = 1;
		writeDescriptorSets[3].pImageInfo = 0;
		writeDescriptorSets[3].pBufferInfo = &rotationBoneChannelDescriptorBufferInfo;
		writeDescriptorSets[3].pTexelBufferView = 0;

		writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[4].pNext = 0;
		writeDescriptorSets[4].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[4].dstBinding = 4;
		writeDescriptorSets[4].dstArrayElement = 0;
		writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[4].descriptorCount = 1;
		writeDescriptorSets[4].pImageInfo = 0;
		writeDescriptorSets[4].pBufferInfo = &scaleBoneChannelDescriptorBufferInfo;
		writeDescriptorSets[4].pTexelBufferView = 0;

		writeDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[5].pNext = 0;
		writeDescriptorSets[5].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[5].dstBinding = 5;
		writeDescriptorSets[5].dstArrayElement = 0;
		writeDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[5].descriptorCount = 1;
		writeDescriptorSets[5].pImageInfo = 0;
		writeDescriptorSets[5].pBufferInfo = &positionKeyFrameDescriptorBufferInfo;
		writeDescriptorSets[5].pTexelBufferView = 0;

		writeDescriptorSets[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[6].pNext = 0;
		writeDescriptorSets[6].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[6].dstBinding = 6;
		writeDescriptorSets[6].dstArrayElement = 0;
		writeDescriptorSets[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[6].descriptorCount = 1;
		writeDescriptorSets[6].pImageInfo = 0;
		writeDescriptorSets[6].pBufferInfo = &rotationKeyFrameDescriptorBufferInfo;
		writeDescriptorSets[6].pTexelBufferView = 0;

		writeDescriptorSets[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[7].pNext = 0;
		writeDescriptorSets[7].dstSet = mDescriptorSets[DescriptorIndex];
		writeDescriptorSets[7].dstBinding = 7;
		writeDescriptorSets[7].dstArrayElement = 0;
		writeDescriptorSets[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[7].descriptorCount = 1;
		writeDescriptorSets[7].pImageInfo = 0;
		writeDescriptorSets[7].pBufferInfo = &scaleKeyFrameDescriptorBufferInfo;
		writeDescriptorSets[7].pTexelBufferView = 0;

		vkUpdateDescriptorSets(gWindow->GetDevice(), (U32)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, 0);
	}

	Buffer* BoneHierarchy::CopyBoneBuffer()
	{
		return BufferVariance::CreateStorage(mBoneBuffer->GetMappedData<void>(), mBoneBuffer->GetSize());
	}

	void BoneHierarchy::Dispatch(U32 DescriptorIndex, AnimatorComponent* AnimatorComponent)
	{
		vkCmdBindPipeline(gRenderer->GetComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline);

		vkCmdBindDescriptorSets(gRenderer->GetComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipelineLayout, 0, 1, &mDescriptorSets[DescriptorIndex], 0, 0);

		static AnimatorState animatorState = {};

		animatorState.Time = AnimatorComponent->GetTime();

		vkCmdPushConstants(gRenderer->GetComputeCommandBuffer(), mPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(AnimatorState), &animatorState);

		vkCmdDispatch(gRenderer->GetComputeCommandBuffer(), MAX_BONES / BONE_HIERARCHY_WORK_GROUP_SIZE, 1, 1);
	}
}