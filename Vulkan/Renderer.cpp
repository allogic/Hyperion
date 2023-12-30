#include <Common/Macros.h>
#include <Common/ValueDatabase.h>

#include <Engine/Scene.h>
#include <Engine/Entity.h>
#include <Engine/Transform.h>
#include <Engine/FontAtlas.h>

#include <Engine/Components/Camera.h>

#include <Vulkan/Image.h>
#include <Vulkan/ImageVariance.h>
#include <Vulkan/Buffer.h>
#include <Vulkan/BufferVariance.h>
#include <Vulkan/Renderer.h>
#include <Vulkan/Window.h>
#include <Vulkan/CommandBuffer.h>
#include <Vulkan/Pipeline.h>

#include <glm/gtc/matrix_transform.hpp>

namespace vulkan
{
	Renderer* gRenderer = 0;

	Renderer::Renderer()
	{
		mTimeInfoBuffer = BufferVariance::CreateUniformCoherent(sizeof(TimeInfo));
		mScreenInfoBuffer = BufferVariance::CreateUniformCoherent(sizeof(ScreenInfo));
		mViewProjectionBuffer = BufferVariance::CreateUniformCoherent(sizeof(ViewProjection));
		mTextVertexBuffer = BufferVariance::CreateVertexCoherent(sizeof(DebugVertex) * 65535);
		mTextIndexBuffer = BufferVariance::CreateIndexCoherent(sizeof(U16) * 65535 * 3);
		mInterfaceVertexBuffer = BufferVariance::CreateVertexCoherent(sizeof(DebugVertex) * 65535);
		mInterfaceIndexBuffer = BufferVariance::CreateIndexCoherent(sizeof(U16) * 65535 * 3);
		mDebugVertexBuffer = BufferVariance::CreateVertexCoherent(sizeof(DebugVertex) * 131070);
		mDebugIndexBuffer = BufferVariance::CreateIndexCoherent(sizeof(U16) * 131070 * 2);

		CreateCommandBuffer();
		CreateSyncObjects();
		CreateRenderPass();
		CreateFrameBuffer();

#ifndef NDEBUG
		CreateQueryPool();
#endif

		mTextDescriptorPool = Pipeline::CreateDescriptorPool(1, mTextDescriptorSetLayoutBindings);
		mTextDescriptorSetLayout = Pipeline::CreateDescriptorSetLayout(mTextDescriptorSetLayoutBindings);
		mTextPipelineLayout = Pipeline::CreatePipelineLayout(mTextDescriptorSetLayout, mTextPushConstantRanges);
		mTextPipeline = Pipeline::CreateGraphicPipeline(mTextPipelineLayout, mRenderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, mTextVertexFile, mTextFragmentFile, mTextVertexInputBindingDescriptions, mTextVertexInputAttributeDescriptions);

		mInterfaceDescriptorPool = Pipeline::CreateDescriptorPool(1, mInterfaceDescriptorSetLayoutBindings);
		mInterfaceDescriptorSetLayout = Pipeline::CreateDescriptorSetLayout(mInterfaceDescriptorSetLayoutBindings);
		mInterfacePipelineLayout = Pipeline::CreatePipelineLayout(mInterfaceDescriptorSetLayout, mInterfacePushConstantRanges);
		mInterfacePipeline = Pipeline::CreateGraphicPipeline(mInterfacePipelineLayout, mRenderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, mInterfaceVertexFile, mInterfaceFragmentFile, mInterfaceVertexInputBindingDescriptions, mInterfaceVertexInputAttributeDescriptions);

		mDebugDescriptorPool = Pipeline::CreateDescriptorPool(1, mDebugDescriptorSetLayoutBindings);
		mDebugDescriptorSetLayout = Pipeline::CreateDescriptorSetLayout(mDebugDescriptorSetLayoutBindings);
		mDebugPipelineLayout = Pipeline::CreatePipelineLayout(mDebugDescriptorSetLayout, mDebugPushConstantRanges);
		mDebugPipeline = Pipeline::CreateGraphicPipeline(mDebugPipelineLayout, mRenderPass, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, mDebugVertexFile, mDebugFragmentFile, mDebugVertexInputBindingDescriptions, mDebugVertexInputAttributeDescriptions);

		BuildTextDescriptorSets(1);
		BuildInterfaceDescriptorSets(1);
		BuildDebugDescriptorSets(1);

		UpdateInterfaceDescriptorSets(0);
		UpdateDebugDescriptorSets(0);
	}

	Renderer::~Renderer()
	{
		DestroyPipeline();
		DestroyPipelineLayout();
		DestroyDescriptorSetLayout();
		DestroyDescriptorPool();

#ifndef NDEBUG
		DestroyQueryPool();
#endif

		DestroyFrameBuffer();
		DestroyRenderPass();
		DestroySyncObjects();
		DestroyCommandBuffer();

		delete mTimeInfoBuffer;
		delete mScreenInfoBuffer;
		delete mViewProjectionBuffer;
		delete mTextVertexBuffer;
		delete mTextIndexBuffer;
		delete mInterfaceVertexBuffer;
		delete mInterfaceIndexBuffer;
		delete mDebugVertexBuffer;
		delete mDebugIndexBuffer;

		delete mFontAtlas;
	}

	void Renderer::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo commandBufferAllocCreateInfo = {};
		commandBufferAllocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocCreateInfo.commandPool = gWindow->GetCommandPool();
		commandBufferAllocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocCreateInfo.commandBufferCount = 1;

		VK_CHECK(vkAllocateCommandBuffers(gWindow->GetDevice(), &commandBufferAllocCreateInfo, &mGraphicCommandBuffer));
		VK_CHECK(vkAllocateCommandBuffers(gWindow->GetDevice(), &commandBufferAllocCreateInfo, &mComputeCommandBuffer));
	}

	void Renderer::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VK_CHECK(vkCreateSemaphore(gWindow->GetDevice(), &semaphoreCreateInfo, 0, &mGraphicCompleteSemaphore));
		VK_CHECK(vkCreateSemaphore(gWindow->GetDevice(), &semaphoreCreateInfo, 0, &mComputeCompleteSemaphore));
		VK_CHECK(vkCreateSemaphore(gWindow->GetDevice(), &semaphoreCreateInfo, 0, &mImageAvailableSemaphore));

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VK_CHECK(vkCreateFence(gWindow->GetDevice(), &fenceCreateInfo, 0, &mRenderFence));
	}

	void Renderer::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachmentDescription = {};
		colorAttachmentDescription.format = gWindow->GetPreferedSurfaceFormat().format;
		colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachmentDescription = {};
		depthAttachmentDescription.format = gWindow->GetDepthFormat();
		depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachmentDescriptions = { colorAttachmentDescription, depthAttachmentDescription };

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.attachmentCount = (U32)attachmentDescriptions.size();
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;
		renderPassCreateInfo.dependencyCount = 1;

		VK_CHECK(vkCreateRenderPass(gWindow->GetDevice(), &renderPassCreateInfo, 0, &mRenderPass));
	}

	void Renderer::CreateFrameBuffer()
	{
		mFrameBuffers.resize(gWindow->GetSwapChainImageCount());

		for (U32 i = 0; i < gWindow->GetSwapChainImageCount(); ++i)
		{
			std::vector<VkImageView> attachments = { gWindow->GetSwapChainImageView(i), gWindow->GetDepthImage()->GetImageView() };

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = mRenderPass;
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.attachmentCount = (U32)attachments.size();
			framebufferCreateInfo.width = gWindow->GetWindowWidth();
			framebufferCreateInfo.height = gWindow->GetWindowHeight();
			framebufferCreateInfo.layers = 1;

			VK_CHECK(vkCreateFramebuffer(gWindow->GetDevice(), &framebufferCreateInfo, 0, &mFrameBuffers[i]));
		}
	}

#ifndef NDEBUG
	void Renderer::CreateQueryPool()
	{
		VkQueryPoolCreateInfo queryPoolCreateInfo = {};
		queryPoolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
		queryPoolCreateInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
		queryPoolCreateInfo.queryCount = 4;

		VK_CHECK(vkCreateQueryPool(gWindow->GetDevice(), &queryPoolCreateInfo, 0, &mQueryPool));

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();
		vkCmdResetQueryPool(commandBuffer, mQueryPool, 0, 4);
		CommandBuffer::End(commandBuffer);
	}
#endif

	void Renderer::DestroyCommandBuffer()
	{
		vkFreeCommandBuffers(gWindow->GetDevice(), gWindow->GetCommandPool(), 1, &mGraphicCommandBuffer);
		vkFreeCommandBuffers(gWindow->GetDevice(), gWindow->GetCommandPool(), 1, &mComputeCommandBuffer);
	}

	void Renderer::DestroySyncObjects()
	{
		vkDestroySemaphore(gWindow->GetDevice(), mGraphicCompleteSemaphore, 0);
		vkDestroySemaphore(gWindow->GetDevice(), mComputeCompleteSemaphore, 0);
		vkDestroySemaphore(gWindow->GetDevice(), mImageAvailableSemaphore, 0);

		vkDestroyFence(gWindow->GetDevice(), mRenderFence, 0);
	}

	void Renderer::DestroyRenderPass()
	{
		vkDestroyRenderPass(gWindow->GetDevice(), mRenderPass, 0);
	}

	void Renderer::DestroyFrameBuffer()
	{
		for (U32 i = 0; i < gWindow->GetSwapChainImageCount(); ++i)
		{
			vkDestroyFramebuffer(gWindow->GetDevice(), mFrameBuffers[i], 0);
		}
	}

	void Renderer::DestroyDescriptorPool()
	{
		vkDestroyDescriptorPool(gWindow->GetDevice(), mTextDescriptorPool, 0);
		vkDestroyDescriptorPool(gWindow->GetDevice(), mInterfaceDescriptorPool, 0);
		vkDestroyDescriptorPool(gWindow->GetDevice(), mDebugDescriptorPool, 0);
	}

#ifndef NDEBUG
	void Renderer::DestroyQueryPool()
	{
		vkDestroyQueryPool(gWindow->GetDevice(), mQueryPool, 0);
	}
#endif

	void Renderer::DestroyDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(gWindow->GetDevice(), mTextDescriptorSetLayout, 0);
		vkDestroyDescriptorSetLayout(gWindow->GetDevice(), mInterfaceDescriptorSetLayout, 0);
		vkDestroyDescriptorSetLayout(gWindow->GetDevice(), mDebugDescriptorSetLayout, 0);
	}

	void Renderer::DestroyPipelineLayout()
	{
		vkDestroyPipelineLayout(gWindow->GetDevice(), mTextPipelineLayout, 0);
		vkDestroyPipelineLayout(gWindow->GetDevice(), mInterfacePipelineLayout, 0);
		vkDestroyPipelineLayout(gWindow->GetDevice(), mDebugPipelineLayout, 0);
	}

	void Renderer::DestroyPipeline()
	{
		vkDestroyPipeline(gWindow->GetDevice(), mTextPipeline, 0);
		vkDestroyPipeline(gWindow->GetDevice(), mInterfacePipeline, 0);
		vkDestroyPipeline(gWindow->GetDevice(), mDebugPipeline, 0);
	}

	void Renderer::BuildTextDescriptorSets(U32 DescriptorCount)
	{
		mTextDescriptorSets.resize(DescriptorCount);

		auto descriptorSetLayouts = std::vector<VkDescriptorSetLayout>{ DescriptorCount, mTextDescriptorSetLayout };

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorSetCount = DescriptorCount;
		descriptorSetAllocateInfo.descriptorPool = mTextDescriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		VK_CHECK(vkAllocateDescriptorSets(gWindow->GetDevice(), &descriptorSetAllocateInfo, mTextDescriptorSets.data()));
	}

	void Renderer::BuildInterfaceDescriptorSets(U32 DescriptorCount)
	{
		mInterfaceDescriptorSets.resize(DescriptorCount);

		auto descriptorSetLayouts = std::vector<VkDescriptorSetLayout>{ DescriptorCount, mInterfaceDescriptorSetLayout };

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorSetCount = DescriptorCount;
		descriptorSetAllocateInfo.descriptorPool = mInterfaceDescriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		VK_CHECK(vkAllocateDescriptorSets(gWindow->GetDevice(), &descriptorSetAllocateInfo, mInterfaceDescriptorSets.data()));
	}

	void Renderer::BuildDebugDescriptorSets(U32 DescriptorCount)
	{
		mDebugDescriptorSets.resize(DescriptorCount);

		auto descriptorSetLayouts = std::vector<VkDescriptorSetLayout>{ DescriptorCount, mDebugDescriptorSetLayout };

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorSetCount = DescriptorCount;
		descriptorSetAllocateInfo.descriptorPool = mDebugDescriptorPool;
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		VK_CHECK(vkAllocateDescriptorSets(gWindow->GetDevice(), &descriptorSetAllocateInfo, mDebugDescriptorSets.data()));
	}

	void Renderer::UpdateTextDescriptorSets(U32 DescriptorIndex)
	{
		VkDescriptorBufferInfo timeInfoDescriptorBufferInfo = {};
		timeInfoDescriptorBufferInfo.offset = 0;
		timeInfoDescriptorBufferInfo.buffer = mTimeInfoBuffer->GetBuffer();
		timeInfoDescriptorBufferInfo.range = mTimeInfoBuffer->GetSize();

		VkDescriptorBufferInfo screenInfoDescriptorBufferInfo = {};
		screenInfoDescriptorBufferInfo.offset = 0;
		screenInfoDescriptorBufferInfo.buffer = mScreenInfoBuffer->GetBuffer();
		screenInfoDescriptorBufferInfo.range = mScreenInfoBuffer->GetSize();

		VkDescriptorBufferInfo viewProjectionDescriptorBufferInfo = {};
		viewProjectionDescriptorBufferInfo.offset = 0;
		viewProjectionDescriptorBufferInfo.buffer = mViewProjectionBuffer->GetBuffer();
		viewProjectionDescriptorBufferInfo.range = mViewProjectionBuffer->GetSize();

		VkDescriptorImageInfo fontAtlasDescriptorImageInfo = {};
		fontAtlasDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		fontAtlasDescriptorImageInfo.imageView = mFontAtlas->GetImageView();
		fontAtlasDescriptorImageInfo.sampler = mFontAtlas->GetSampler();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {};
		writeDescriptorSets.resize(mTextDescriptorSetLayoutBindings.size());

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].pNext = 0;
		writeDescriptorSets[0].dstSet = mTextDescriptorSets[DescriptorIndex];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pImageInfo = 0;
		writeDescriptorSets[0].pBufferInfo = &timeInfoDescriptorBufferInfo;
		writeDescriptorSets[0].pTexelBufferView = 0;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].pNext = 0;
		writeDescriptorSets[1].dstSet = mTextDescriptorSets[DescriptorIndex];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = 0;
		writeDescriptorSets[1].pBufferInfo = &screenInfoDescriptorBufferInfo;
		writeDescriptorSets[1].pTexelBufferView = 0;

		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].pNext = 0;
		writeDescriptorSets[2].dstSet = mTextDescriptorSets[DescriptorIndex];
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].dstArrayElement = 0;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].pImageInfo = 0;
		writeDescriptorSets[2].pBufferInfo = &viewProjectionDescriptorBufferInfo;
		writeDescriptorSets[2].pTexelBufferView = 0;

		writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[3].pNext = 0;
		writeDescriptorSets[3].dstSet = mTextDescriptorSets[DescriptorIndex];
		writeDescriptorSets[3].dstBinding = 3;
		writeDescriptorSets[3].dstArrayElement = 0;
		writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[3].descriptorCount = 1;
		writeDescriptorSets[3].pImageInfo = &fontAtlasDescriptorImageInfo;
		writeDescriptorSets[3].pBufferInfo = 0;
		writeDescriptorSets[3].pTexelBufferView = 0;

		vkUpdateDescriptorSets(gWindow->GetDevice(), (U32)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, 0);
	}

	void Renderer::UpdateInterfaceDescriptorSets(U32 DescriptorIndex)
	{
		VkDescriptorBufferInfo timeInfoDescriptorBufferInfo = {};
		timeInfoDescriptorBufferInfo.offset = 0;
		timeInfoDescriptorBufferInfo.buffer = mTimeInfoBuffer->GetBuffer();
		timeInfoDescriptorBufferInfo.range = mTimeInfoBuffer->GetSize();

		VkDescriptorBufferInfo screenInfoDescriptorBufferInfo = {};
		screenInfoDescriptorBufferInfo.offset = 0;
		screenInfoDescriptorBufferInfo.buffer = mScreenInfoBuffer->GetBuffer();
		screenInfoDescriptorBufferInfo.range = mScreenInfoBuffer->GetSize();

		VkDescriptorBufferInfo viewProjectionDescriptorBufferInfo = {};
		viewProjectionDescriptorBufferInfo.offset = 0;
		viewProjectionDescriptorBufferInfo.buffer = mViewProjectionBuffer->GetBuffer();
		viewProjectionDescriptorBufferInfo.range = mViewProjectionBuffer->GetSize();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {};
		writeDescriptorSets.resize(mInterfaceDescriptorSetLayoutBindings.size());

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].pNext = 0;
		writeDescriptorSets[0].dstSet = mInterfaceDescriptorSets[DescriptorIndex];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pImageInfo = 0;
		writeDescriptorSets[0].pBufferInfo = &timeInfoDescriptorBufferInfo;
		writeDescriptorSets[0].pTexelBufferView = 0;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].pNext = 0;
		writeDescriptorSets[1].dstSet = mInterfaceDescriptorSets[DescriptorIndex];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = 0;
		writeDescriptorSets[1].pBufferInfo = &screenInfoDescriptorBufferInfo;
		writeDescriptorSets[1].pTexelBufferView = 0;

		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].pNext = 0;
		writeDescriptorSets[2].dstSet = mInterfaceDescriptorSets[DescriptorIndex];
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].dstArrayElement = 0;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].pImageInfo = 0;
		writeDescriptorSets[2].pBufferInfo = &viewProjectionDescriptorBufferInfo;
		writeDescriptorSets[2].pTexelBufferView = 0;

		vkUpdateDescriptorSets(gWindow->GetDevice(), (U32)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, 0);
	}

	void Renderer::UpdateDebugDescriptorSets(U32 DescriptorIndex)
	{
		VkDescriptorBufferInfo timeInfoDescriptorBufferInfo = {};
		timeInfoDescriptorBufferInfo.offset = 0;
		timeInfoDescriptorBufferInfo.buffer = mTimeInfoBuffer->GetBuffer();
		timeInfoDescriptorBufferInfo.range = mTimeInfoBuffer->GetSize();

		VkDescriptorBufferInfo viewProjectionDescriptorBufferInfo = {};
		viewProjectionDescriptorBufferInfo.offset = 0;
		viewProjectionDescriptorBufferInfo.buffer = mViewProjectionBuffer->GetBuffer();
		viewProjectionDescriptorBufferInfo.range = mViewProjectionBuffer->GetSize();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {};
		writeDescriptorSets.resize(mDebugDescriptorSetLayoutBindings.size());

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].pNext = 0;
		writeDescriptorSets[0].dstSet = mDebugDescriptorSets[DescriptorIndex];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pImageInfo = 0;
		writeDescriptorSets[0].pBufferInfo = &timeInfoDescriptorBufferInfo;
		writeDescriptorSets[0].pTexelBufferView = 0;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].pNext = 0;
		writeDescriptorSets[1].dstSet = mDebugDescriptorSets[DescriptorIndex];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = 0;
		writeDescriptorSets[1].pBufferInfo = &viewProjectionDescriptorBufferInfo;
		writeDescriptorSets[1].pTexelBufferView = 0;

		vkUpdateDescriptorSets(gWindow->GetDevice(), (U32)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, 0);
	}

	void Renderer::BuildGraphicCommandBuffer(Scene* Scene, U32 ImageIndex)
	{
		gWindow->GraphicQueueWaitIdle();

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = 0;

		VK_CHECK(vkBeginCommandBuffer(mGraphicCommandBuffer, &commandBufferBeginInfo));

		VkClearValue colorClearValue = {};
		colorClearValue.color.float32[0] = 0.0F;
		colorClearValue.color.float32[1] = 0.0F;
		colorClearValue.color.float32[2] = 0.0F;
		colorClearValue.color.float32[3] = 1.0F;

		VkClearValue depthClearValue = {};
		depthClearValue.depthStencil.depth = 1.0F;
		depthClearValue.depthStencil.stencil = 0;

		std::vector<VkClearValue> clearValues = { colorClearValue, depthClearValue };

		VkRenderPassBeginInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassCreateInfo.renderPass = mRenderPass;
		renderPassCreateInfo.framebuffer = mFrameBuffers[ImageIndex];
		renderPassCreateInfo.renderArea.offset.x = 0;
		renderPassCreateInfo.renderArea.offset.y = 0;
		renderPassCreateInfo.renderArea.extent.width = gWindow->GetWindowWidth();
		renderPassCreateInfo.renderArea.extent.height = gWindow->GetWindowHeight();
		renderPassCreateInfo.pClearValues = clearValues.data();
		renderPassCreateInfo.clearValueCount = (U32)clearValues.size();

		VkViewport viewport = {};
		viewport.x = 0.0F;
		viewport.y = 0.0F;
		viewport.width = (R32)gWindow->GetWindowWidth();
		viewport.height = (R32)gWindow->GetWindowHeight();
		viewport.minDepth = 0.0F;
		viewport.maxDepth = 1.0F;

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = gWindow->GetWindowWidth();
		scissor.extent.height = gWindow->GetWindowHeight();

		vkCmdBeginRenderPass(mGraphicCommandBuffer, &renderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdSetViewport(mGraphicCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(mGraphicCommandBuffer, 0, 1, &scissor);

#ifndef NDEBUG
		vkCmdWriteTimestamp(mGraphicCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, mQueryPool, 0);
#endif

		{
			vkCmdBindPipeline(mGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mDebugPipeline);

			std::vector<VkBuffer> defaultVertexBuffers = { mDebugVertexBuffer->GetBuffer() };
			std::vector<U64> defaultOffsets = { 0 };

			vkCmdBindDescriptorSets(mGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mDebugPipelineLayout, 0, 1, &mDebugDescriptorSets[0], 0, 0);
			vkCmdBindVertexBuffers(mGraphicCommandBuffer, DebugVertexBindingId, 1, defaultVertexBuffers.data(), defaultOffsets.data());
			vkCmdBindIndexBuffer(mGraphicCommandBuffer, mDebugIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

			//vkCmdPushConstants(mGraphicCommandBuffer, mDebugPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PerEntityData), &perEntityData);

			vkCmdDrawIndexed(mGraphicCommandBuffer, mDebugIndexCount, 1, 0, 0, 0);
		}

		{
			vkCmdBindPipeline(mGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mInterfacePipeline);

			std::vector<VkBuffer> defaultVertexBuffers = { mInterfaceVertexBuffer->GetBuffer() };
			std::vector<U64> defaultOffsets = { 0 };

			vkCmdBindDescriptorSets(mGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mInterfacePipelineLayout, 0, 1, &mInterfaceDescriptorSets[0], 0, 0);
			vkCmdBindVertexBuffers(mGraphicCommandBuffer, InterfaceVertexBindingId, 1, defaultVertexBuffers.data(), defaultOffsets.data());
			vkCmdBindIndexBuffer(mGraphicCommandBuffer, mInterfaceIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

			//vkCmdPushConstants(mGraphicCommandBuffer, mInterfacePipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PerEntityData), &perEntityData);

			vkCmdDrawIndexed(mGraphicCommandBuffer, mInterfaceIndexCount, 1, 0, 0, 0);
		}

		{
			vkCmdBindPipeline(mGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mTextPipeline);

			std::vector<VkBuffer> defaultVertexBuffers = { mTextVertexBuffer->GetBuffer() };
			std::vector<U64> defaultOffsets = { 0 };

			vkCmdBindDescriptorSets(mGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mTextPipelineLayout, 0, 1, &mTextDescriptorSets[0], 0, 0);
			vkCmdBindVertexBuffers(mGraphicCommandBuffer, TextVertexBindingId, 1, defaultVertexBuffers.data(), defaultOffsets.data());
			vkCmdBindIndexBuffer(mGraphicCommandBuffer, mTextIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

			//vkCmdPushConstants(mGraphicCommandBuffer, mTextPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PerEntityData), &perEntityData);

			vkCmdDrawIndexed(mGraphicCommandBuffer, mTextIndexCount, 1, 0, 0, 0);
		}

#ifndef NDEBUG
		vkCmdWriteTimestamp(mGraphicCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, mQueryPool, 1);
#endif

		vkCmdEndRenderPass(mGraphicCommandBuffer);

		VK_CHECK(vkEndCommandBuffer(mGraphicCommandBuffer));
	}

	void Renderer::BuildComputeCommandBuffer(Scene* Scene)
	{
		gWindow->ComputeQueueWaitIdle();

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = 0;

		VK_CHECK(vkBeginCommandBuffer(mComputeCommandBuffer, &commandBufferBeginInfo));

#ifndef NDEBUG
		vkCmdWriteTimestamp(mComputeCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, mQueryPool, 2);
#endif

		Scene->DispatchTransformHierarchy();

#ifndef NDEBUG
		vkCmdWriteTimestamp(mComputeCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, mQueryPool, 3);
#endif

		VK_CHECK(vkEndCommandBuffer(mComputeCommandBuffer));
	}

	void Renderer::ResizeBefore()
	{
		DestroyPipeline();

		DestroyFrameBuffer();
		DestroyRenderPass();
	}

	void Renderer::ResizeAfter()
	{
		CreateRenderPass();
		CreateFrameBuffer();

		mTextPipeline = Pipeline::CreateGraphicPipeline(mTextPipelineLayout, mRenderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, mTextVertexFile, mTextFragmentFile, mTextVertexInputBindingDescriptions, mTextVertexInputAttributeDescriptions);
		mInterfacePipeline = Pipeline::CreateGraphicPipeline(mInterfacePipelineLayout, mRenderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, mInterfaceVertexFile, mInterfaceFragmentFile, mInterfaceVertexInputBindingDescriptions, mInterfaceVertexInputAttributeDescriptions);
		mDebugPipeline = Pipeline::CreateGraphicPipeline(mDebugPipelineLayout, mRenderPass, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, mDebugVertexFile, mDebugFragmentFile, mDebugVertexInputBindingDescriptions, mDebugVertexInputAttributeDescriptions);
	}

	void Renderer::Render(Scene* Scene)
	{
		VK_CHECK(vkResetFences(gWindow->GetDevice(), 1, &gRenderer->mRenderFence));

		gRenderer->mTimeInfo.Time = gWindow->GetTime();
		gRenderer->mTimeInfo.DeltaTime = gWindow->GetDeltaTime();

		gRenderer->mScreenInfo.Size = R32V2{ (R32)gWindow->GetWindowWidth(), (R32)gWindow->GetWindowHeight() };

		if (Scene)
		{
			if (Entity* player = Scene->GetPlayer())
			{
				Transform* transform = Scene->GetTransform(player);
				Camera* camera = Scene->GetCamera(player);

				gRenderer->mViewProjection.View = glm::lookAt(transform->WorldPosition, transform->WorldPosition + transform->LocalFront, WorldDown);
				gRenderer->mViewProjection.Projection = glm::perspective(camera->GetFov(), gWindow->GetAspectRatio(), camera->GetNear(), camera->GetFar());
			}
		}

		gRenderer->mTimeInfoBuffer->SetMappedData(&gRenderer->mTimeInfo, sizeof(TimeInfo));
		gRenderer->mScreenInfoBuffer->SetMappedData(&gRenderer->mScreenInfo, sizeof(ScreenInfo));
		gRenderer->mViewProjectionBuffer->SetMappedData(&gRenderer->mViewProjection, sizeof(ViewProjection));

		U32 imageIndex;
		VK_CHECK(vkAcquireNextImageKHR(gWindow->GetDevice(), gWindow->GetSwapChain(), UINT64_MAX, gRenderer->mImageAvailableSemaphore, 0, &imageIndex));

		gRenderer->BuildComputeCommandBuffer(Scene);
		gRenderer->BuildGraphicCommandBuffer(Scene, imageIndex);

		{
			std::vector<VkSemaphore> signalSemaphores = { gRenderer->mComputeCompleteSemaphore };

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pWaitSemaphores = 0;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = signalSemaphores.data();
			submitInfo.signalSemaphoreCount = (U32)signalSemaphores.size();
			submitInfo.pWaitDstStageMask = 0;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &gRenderer->mComputeCommandBuffer;

			VK_CHECK(vkQueueSubmit(gWindow->GetComputeQueue(), 1, &submitInfo, 0));
		}

		{
			std::vector<VkSemaphore> waitSemaphores = { gRenderer->mComputeCompleteSemaphore, gRenderer->mImageAvailableSemaphore };
			std::vector<VkSemaphore> signalSemaphores = { gRenderer->mGraphicCompleteSemaphore };
			std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pWaitSemaphores = waitSemaphores.data();
			submitInfo.waitSemaphoreCount = (U32)waitSemaphores.size();
			submitInfo.pSignalSemaphores = signalSemaphores.data();
			submitInfo.signalSemaphoreCount = (U32)signalSemaphores.size();
			submitInfo.pWaitDstStageMask = waitStages.data();
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &gRenderer->mGraphicCommandBuffer;

			VK_CHECK(vkQueueSubmit(gWindow->GetGraphicQueue(), 1, &submitInfo, gRenderer->mRenderFence));
		}

		VK_CHECK(vkWaitForFences(gWindow->GetDevice(), 1, &gRenderer->mRenderFence, VK_TRUE, UINT64_MAX));

		{
			std::vector<VkSemaphore> waitSemaphores = { gRenderer->mGraphicCompleteSemaphore };
			std::vector<VkSwapchainKHR> swapChains = { gWindow->GetSwapChain() };

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pWaitSemaphores = waitSemaphores.data();
			presentInfo.waitSemaphoreCount = (U32)waitSemaphores.size();
			presentInfo.pSwapchains = swapChains.data();
			presentInfo.swapchainCount = (U32)swapChains.size();
			presentInfo.pImageIndices = &imageIndex;

			VK_CHECK(vkQueuePresentKHR(gWindow->GetPresentQueue(), &presentInfo));
		}

#ifndef NDEBUG
		std::array<U64, 4> queryResults = {};

		vkGetQueryPoolResults(gWindow->GetDevice(), gRenderer->mQueryPool, 0, 4, sizeof(U64) * queryResults.size(), queryResults.data(), sizeof(U64), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);

		VkCommandBuffer commandBuffer = CommandBuffer::Begin();
		vkCmdResetQueryPool(commandBuffer, gRenderer->mQueryPool, 0, 4);
		CommandBuffer::End(commandBuffer);

		R32 timestampPeriod = gWindow->GetPhysicalDeviceProperties().limits.timestampPeriod;

		R32 graphicElapsedTimeNanoSeconds = (queryResults[1] - queryResults[0]) * timestampPeriod;
		R32 computeElapsedTimeNanoSeconds = (queryResults[3] - queryResults[2]) * timestampPeriod;

		ValueDatabase::SetValue("graphicTime", (R32)graphicElapsedTimeNanoSeconds);
		ValueDatabase::SetValue("computeTime", (R32)computeElapsedTimeNanoSeconds);
#endif

		gRenderer->mTextVertexCount = 0;
		gRenderer->mTextIndexCount = 0;

		gRenderer->mInterfaceVertexCount = 0;
		gRenderer->mInterfaceIndexCount = 0;

		gRenderer->mDebugVertexCount = 0;
		gRenderer->mDebugIndexCount = 0;
	}

	void Renderer::UpdateFontAtlas(U32 Width, U32 Height, std::vector<U8> const& Atlas)
	{
		if (mFontAtlas)
		{
			delete mFontAtlas;
		}

		mFontAtlas = ImageVariance::CreateRImage2D(Width, Height, (void*)Atlas.data());
	}

	void Renderer::SetFontInfo(std::string const& Font, FontInfo const& Info)
	{
		mFontInfos[Font] = Info;
	}

	R32 Renderer::GetTextWidth(std::string const& Font, std::string const& Text, R32 Scale, R32* Value)
	{
		R32 textWidth = 0.0F;

		static char buffer[32] = {};

		if (Value)
		{
			std::memset(buffer, 0, sizeof(buffer));

			U32 bufferSize = std::sprintf(buffer, Text.data(), *Value);

			if (bufferSize)
			{
				for (U32 i = 0; i < bufferSize; ++i)
				{
					U8 c = buffer[i];

					auto const& fontInfo = gRenderer->mFontInfos[Font];
					auto const& characterInfo = fontInfo.CharacterInfos[c];

					textWidth += (characterInfo.Advance >> 6) * Scale;
				}
			}
		}
		else
		{
			for (U32 i = 0; i < Text.size(); ++i)
			{
				U8 c = Text[i];

				auto const& fontInfo = gRenderer->mFontInfos[Font];
				auto const& characterInfo = fontInfo.CharacterInfos[c];

				textWidth += (characterInfo.Advance >> 6) * Scale;
			}
		}

		return textWidth;
	}

	void Renderer::DrawText(R32V3 const& Position, std::string const& Font, std::string const& Text, R32 Scale, U32 Color, R32* Value)
	{
		TextVertex* vertices = gRenderer->mTextVertexBuffer->GetMappedData<TextVertex>();
		U16* indices = gRenderer->mTextIndexBuffer->GetMappedData<U16>();

		R32 horizontalOffset = 0.0F;

		U32 fontCount = (U32)gRenderer->mFontInfos.size();

		static char buffer[32] = {};

		if (Value)
		{
			std::memset(buffer, 0, sizeof(buffer));

			U32 bufferSize = std::sprintf(buffer, Text.data(), *Value);

			if (bufferSize)
			{
				for (U32 i = 0; i < bufferSize; ++i)
				{
					U8 c = buffer[i];

					auto const& fontInfo = gRenderer->mFontInfos[Font];
					auto const& characterInfo = fontInfo.CharacterInfos[c];

					R32 x = Position.x + horizontalOffset + characterInfo.Bearing.x * Scale;
					R32 y = Position.y - (characterInfo.Size.y - characterInfo.Bearing.y) * Scale;
					R32 z = Position.z;

					R32 w = characterInfo.Size.x * Scale;
					R32 h = characterInfo.Size.y * Scale;

					R32V2 atlasPos = { 1.0F / fontInfo.FontSize.x * fontInfo.CharacterSize.x * c, 1.0F / fontCount * fontInfo.FontIndex };
					R32V2 atlasSize = { 1.0F / fontInfo.FontSize.x * characterInfo.Size.x, 1.0F / fontCount / fontInfo.FontSize.y * characterInfo.Size.y };

					vertices[gRenderer->mTextVertexCount + 0].Position = R32V3{ x, y, z };
					vertices[gRenderer->mTextVertexCount + 1].Position = R32V3{ x + w, y, z };
					vertices[gRenderer->mTextVertexCount + 2].Position = R32V3{ x, y + h, z };
					vertices[gRenderer->mTextVertexCount + 3].Position = R32V3{ x + w, y + h, z };

					vertices[gRenderer->mTextVertexCount + 0].Uv = R32V2{ atlasPos.x, atlasPos.y + atlasSize.y };
					vertices[gRenderer->mTextVertexCount + 1].Uv = R32V2{ atlasPos.x + atlasSize.x, atlasPos.y + atlasSize.y };
					vertices[gRenderer->mTextVertexCount + 2].Uv = R32V2{ atlasPos.x, atlasPos.y };
					vertices[gRenderer->mTextVertexCount + 3].Uv = R32V2{ atlasPos.x + atlasSize.x, atlasPos.y };

					vertices[gRenderer->mTextVertexCount + 0].Color = Color;
					vertices[gRenderer->mTextVertexCount + 1].Color = Color;
					vertices[gRenderer->mTextVertexCount + 2].Color = Color;
					vertices[gRenderer->mTextVertexCount + 3].Color = Color;

					indices[gRenderer->mTextIndexCount + 0] = gRenderer->mTextVertexCount + 0;
					indices[gRenderer->mTextIndexCount + 1] = gRenderer->mTextVertexCount + 2;
					indices[gRenderer->mTextIndexCount + 2] = gRenderer->mTextVertexCount + 1;

					indices[gRenderer->mTextIndexCount + 3] = gRenderer->mTextVertexCount + 2;
					indices[gRenderer->mTextIndexCount + 4] = gRenderer->mTextVertexCount + 3;
					indices[gRenderer->mTextIndexCount + 5] = gRenderer->mTextVertexCount + 1;

					gRenderer->mTextVertexCount += 4;
					gRenderer->mTextIndexCount += 6;

					horizontalOffset += (characterInfo.Advance >> 6) * Scale;
				}
			}
		}
		else
		{
			for (U32 i = 0; i < Text.size(); ++i)
			{
				U8 c = Text[i];

				auto const& fontInfo = gRenderer->mFontInfos[Font];
				auto const& characterInfo = fontInfo.CharacterInfos[c];

				R32 x = Position.x + horizontalOffset + characterInfo.Bearing.x * Scale;
				R32 y = Position.y - (characterInfo.Size.y - characterInfo.Bearing.y) * Scale;
				R32 z = Position.z;

				R32 w = characterInfo.Size.x * Scale;
				R32 h = characterInfo.Size.y * Scale;

				R32V2 atlasPos = { 1.0F / fontInfo.FontSize.x * fontInfo.CharacterSize.x * c, 1.0F / fontCount * fontInfo.FontIndex };
				R32V2 atlasSize = { 1.0F / fontInfo.FontSize.x * characterInfo.Size.x, 1.0F / fontCount / fontInfo.FontSize.y * characterInfo.Size.y };

				vertices[gRenderer->mTextVertexCount + 0].Position = R32V3{ x, y, z };
				vertices[gRenderer->mTextVertexCount + 1].Position = R32V3{ x + w, y, z };
				vertices[gRenderer->mTextVertexCount + 2].Position = R32V3{ x, y + h, z };
				vertices[gRenderer->mTextVertexCount + 3].Position = R32V3{ x + w, y + h, z };

				vertices[gRenderer->mTextVertexCount + 0].Uv = R32V2{ atlasPos.x, atlasPos.y + atlasSize.y };
				vertices[gRenderer->mTextVertexCount + 1].Uv = R32V2{ atlasPos.x + atlasSize.x, atlasPos.y + atlasSize.y };
				vertices[gRenderer->mTextVertexCount + 2].Uv = R32V2{ atlasPos.x, atlasPos.y };
				vertices[gRenderer->mTextVertexCount + 3].Uv = R32V2{ atlasPos.x + atlasSize.x, atlasPos.y };

				vertices[gRenderer->mTextVertexCount + 0].Color = Color;
				vertices[gRenderer->mTextVertexCount + 1].Color = Color;
				vertices[gRenderer->mTextVertexCount + 2].Color = Color;
				vertices[gRenderer->mTextVertexCount + 3].Color = Color;

				indices[gRenderer->mTextIndexCount + 0] = gRenderer->mTextVertexCount + 0;
				indices[gRenderer->mTextIndexCount + 1] = gRenderer->mTextVertexCount + 2;
				indices[gRenderer->mTextIndexCount + 2] = gRenderer->mTextVertexCount + 1;

				indices[gRenderer->mTextIndexCount + 3] = gRenderer->mTextVertexCount + 2;
				indices[gRenderer->mTextIndexCount + 4] = gRenderer->mTextVertexCount + 3;
				indices[gRenderer->mTextIndexCount + 5] = gRenderer->mTextVertexCount + 1;

				gRenderer->mTextVertexCount += 4;
				gRenderer->mTextIndexCount += 6;

				horizontalOffset += (characterInfo.Advance >> 6) * Scale;
			}
		}
	}

	void Renderer::DrawInterfaceRect(R32V3 const& Position, R32V2 const& Size, U32 Color)
	{
		InterfaceVertex* vertices = gRenderer->mInterfaceVertexBuffer->GetMappedData<InterfaceVertex>();

		vertices[gRenderer->mInterfaceVertexCount + 0].Position = R32V3{ Position.x, Position.y, Position.z };
		vertices[gRenderer->mInterfaceVertexCount + 1].Position = R32V3{ Position.x + Size.x, Position.y, Position.z };
		vertices[gRenderer->mInterfaceVertexCount + 2].Position = R32V3{ Position.x, Position.y + Size.y, Position.z };
		vertices[gRenderer->mInterfaceVertexCount + 3].Position = R32V3{ Position.x + Size.x, Position.y + Size.y, Position.z };

		vertices[gRenderer->mInterfaceVertexCount + 0].Uv = R32V2{ 0.0F, 0.0F };
		vertices[gRenderer->mInterfaceVertexCount + 1].Uv = R32V2{ 1.0F, 0.0F };
		vertices[gRenderer->mInterfaceVertexCount + 2].Uv = R32V2{ 0.0F, 1.0F };
		vertices[gRenderer->mInterfaceVertexCount + 3].Uv = R32V2{ 1.0F, 1.0F };

		vertices[gRenderer->mInterfaceVertexCount + 0].Color = Color;
		vertices[gRenderer->mInterfaceVertexCount + 1].Color = Color;
		vertices[gRenderer->mInterfaceVertexCount + 2].Color = Color;
		vertices[gRenderer->mInterfaceVertexCount + 3].Color = Color;

		U16* indices = gRenderer->mInterfaceIndexBuffer->GetMappedData<U16>();

		indices[gRenderer->mInterfaceIndexCount + 0] = gRenderer->mInterfaceVertexCount + 0;
		indices[gRenderer->mInterfaceIndexCount + 1] = gRenderer->mInterfaceVertexCount + 2;
		indices[gRenderer->mInterfaceIndexCount + 2] = gRenderer->mInterfaceVertexCount + 1;

		indices[gRenderer->mInterfaceIndexCount + 3] = gRenderer->mInterfaceVertexCount + 2;
		indices[gRenderer->mInterfaceIndexCount + 4] = gRenderer->mInterfaceVertexCount + 3;
		indices[gRenderer->mInterfaceIndexCount + 5] = gRenderer->mInterfaceVertexCount + 1;

		gRenderer->mInterfaceVertexCount += 4;
		gRenderer->mInterfaceIndexCount += 6;
	}

	void Renderer::DrawDebugLine(R32V3 const& From, R32V3 const& To, U32 Color)
	{
		DebugVertex* vertices = gRenderer->mDebugVertexBuffer->GetMappedData<DebugVertex>();

		vertices[gRenderer->mDebugVertexCount + 0].Position = From;
		vertices[gRenderer->mDebugVertexCount + 1].Position = To;

		vertices[gRenderer->mDebugVertexCount + 0].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 1].Color = Color;

		U16* indices = gRenderer->mDebugIndexBuffer->GetMappedData<U16>();

		indices[gRenderer->mDebugIndexCount + 0] = gRenderer->mDebugVertexCount + 0;
		indices[gRenderer->mDebugIndexCount + 1] = gRenderer->mDebugVertexCount + 1;

		gRenderer->mDebugVertexCount += 2;
		gRenderer->mDebugIndexCount += 2;
	}

	void Renderer::DrawDebugRectXY(R32V3 const& Position, R32V3 const& Size, U32 Color, R32Q const& Rotation)
	{
		DebugVertex* vertices = gRenderer->mDebugVertexBuffer->GetMappedData<DebugVertex>();

		R32V3 half = Size / 2.0F;

		vertices[gRenderer->mDebugVertexCount + 0].Position = Position + R32V3{ -half.x, -half.y, 0.0F } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 1].Position = Position + R32V3{  half.x, -half.y, 0.0F } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 2].Position = Position + R32V3{ -half.x,  half.y, 0.0F } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 3].Position = Position + R32V3{  half.x,  half.y, 0.0F } * Rotation;

		vertices[gRenderer->mDebugVertexCount + 0].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 1].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 2].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 3].Color = Color;

		U16* indices = gRenderer->mDebugIndexBuffer->GetMappedData<U16>();

		indices[gRenderer->mDebugIndexCount + 0] = gRenderer->mDebugVertexCount + 0;
		indices[gRenderer->mDebugIndexCount + 1] = gRenderer->mDebugVertexCount + 1;
		indices[gRenderer->mDebugIndexCount + 2] = gRenderer->mDebugVertexCount + 1;
		indices[gRenderer->mDebugIndexCount + 3] = gRenderer->mDebugVertexCount + 3;
		indices[gRenderer->mDebugIndexCount + 4] = gRenderer->mDebugVertexCount + 3;
		indices[gRenderer->mDebugIndexCount + 5] = gRenderer->mDebugVertexCount + 2;
		indices[gRenderer->mDebugIndexCount + 6] = gRenderer->mDebugVertexCount + 2;
		indices[gRenderer->mDebugIndexCount + 7] = gRenderer->mDebugVertexCount + 0;

		gRenderer->mDebugVertexCount += 4;
		gRenderer->mDebugIndexCount += 8;
	}

	void Renderer::DrawDebugRectXZ(R32V3 const& Position, R32V3 const& Size, U32 Color, R32Q const& Rotation)
	{
		DebugVertex* vertices = gRenderer->mDebugVertexBuffer->GetMappedData<DebugVertex>();

		R32V3 half = Size / 2.0F;

		vertices[gRenderer->mDebugVertexCount + 0].Position = Position + R32V3{ -half.x, 0.0F, -half.z } *Rotation;
		vertices[gRenderer->mDebugVertexCount + 1].Position = Position + R32V3{  half.x, 0.0F, -half.z } *Rotation;
		vertices[gRenderer->mDebugVertexCount + 2].Position = Position + R32V3{ -half.x, 0.0F,  half.z } *Rotation;
		vertices[gRenderer->mDebugVertexCount + 3].Position = Position + R32V3{  half.x, 0.0F,  half.z } *Rotation;

		vertices[gRenderer->mDebugVertexCount + 0].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 1].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 2].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 3].Color = Color;

		U16* indices = gRenderer->mDebugIndexBuffer->GetMappedData<U16>();

		indices[gRenderer->mDebugIndexCount + 0] = gRenderer->mDebugVertexCount + 0;
		indices[gRenderer->mDebugIndexCount + 1] = gRenderer->mDebugVertexCount + 1;
		indices[gRenderer->mDebugIndexCount + 2] = gRenderer->mDebugVertexCount + 1;
		indices[gRenderer->mDebugIndexCount + 3] = gRenderer->mDebugVertexCount + 3;
		indices[gRenderer->mDebugIndexCount + 4] = gRenderer->mDebugVertexCount + 3;
		indices[gRenderer->mDebugIndexCount + 5] = gRenderer->mDebugVertexCount + 2;
		indices[gRenderer->mDebugIndexCount + 6] = gRenderer->mDebugVertexCount + 2;
		indices[gRenderer->mDebugIndexCount + 7] = gRenderer->mDebugVertexCount + 0;

		gRenderer->mDebugVertexCount += 4;
		gRenderer->mDebugIndexCount += 8;
	}

	void Renderer::DrawDebugGridXZ(R32V3 const& Position, U32 DivisionX, U32 DivisionZ, R32 Size, U32 Color, R32Q const& Rotation)
	{
		DebugVertex* vertices = gRenderer->mDebugVertexBuffer->GetMappedData<DebugVertex>();
		U16* indices = gRenderer->mDebugIndexBuffer->GetMappedData<U16>();

		R32 half = Size / 2.0F;
		R32 fractX = Size / DivisionX;
		R32 fractZ = Size / DivisionZ;

		for (U32 x = 0; x < (DivisionX + 1); ++x)
		{
			R32 positionZ = fractZ * x - half;

			vertices[gRenderer->mDebugVertexCount + 0].Position = Position + R32V3{ -half, 0.0F, positionZ } * Rotation;
			vertices[gRenderer->mDebugVertexCount + 1].Position = Position + R32V3{  half, 0.0F, positionZ } * Rotation;

			vertices[gRenderer->mDebugVertexCount + 0].Color = Color;
			vertices[gRenderer->mDebugVertexCount + 1].Color = Color;

			indices[gRenderer->mDebugIndexCount + 0] = gRenderer->mDebugVertexCount + 0;
			indices[gRenderer->mDebugIndexCount + 1] = gRenderer->mDebugVertexCount + 1;

			gRenderer->mDebugVertexCount += 2;
			gRenderer->mDebugIndexCount += 2;
		}

		for (U32 z = 0; z < (DivisionZ + 1); ++z)
		{
			R32 positionX = fractX * z - half;

			vertices[gRenderer->mDebugVertexCount + 0].Position = Position + R32V3{ positionX, 0.0F, -half } * Rotation;
			vertices[gRenderer->mDebugVertexCount + 1].Position = Position + R32V3{ positionX, 0.0F,  half } * Rotation;

			vertices[gRenderer->mDebugVertexCount + 0].Color = Color;
			vertices[gRenderer->mDebugVertexCount + 1].Color = Color;

			indices[gRenderer->mDebugIndexCount + 0] = gRenderer->mDebugVertexCount + 0;
			indices[gRenderer->mDebugIndexCount + 1] = gRenderer->mDebugVertexCount + 1;

			gRenderer->mDebugVertexCount += 2;
			gRenderer->mDebugIndexCount += 2;
		}
	}

	void Renderer::DrawDebugBox(R32V3 const& Position, R32V3 const& Size, U32 Color, R32Q const& Rotation)
	{
		DebugVertex* vertices = gRenderer->mDebugVertexBuffer->GetMappedData<DebugVertex>();

		R32V3 half = Size / 2.0F;

		vertices[gRenderer->mDebugVertexCount + 0].Position = Position + R32V3{ -half.x, -half.y, -half.z } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 1].Position = Position + R32V3{  half.x, -half.y, -half.z } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 2].Position = Position + R32V3{ -half.x,  half.y, -half.z } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 3].Position = Position + R32V3{  half.x,  half.y, -half.z } * Rotation;

		vertices[gRenderer->mDebugVertexCount + 4].Position = Position + R32V3{ -half.x, -half.y,  half.z } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 5].Position = Position + R32V3{  half.x, -half.y,  half.z } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 6].Position = Position + R32V3{ -half.x,  half.y,  half.z } * Rotation;
		vertices[gRenderer->mDebugVertexCount + 7].Position = Position + R32V3{  half.x,  half.y,  half.z } * Rotation;

		vertices[gRenderer->mDebugVertexCount + 0].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 1].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 2].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 3].Color = Color;

		vertices[gRenderer->mDebugVertexCount + 4].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 5].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 6].Color = Color;
		vertices[gRenderer->mDebugVertexCount + 7].Color = Color;

		U16* indices = gRenderer->mDebugIndexBuffer->GetMappedData<U16>();

		indices[gRenderer->mDebugIndexCount + 0] = gRenderer->mDebugVertexCount + 0;
		indices[gRenderer->mDebugIndexCount + 1] = gRenderer->mDebugVertexCount + 1;
		indices[gRenderer->mDebugIndexCount + 2] = gRenderer->mDebugVertexCount + 1;
		indices[gRenderer->mDebugIndexCount + 3] = gRenderer->mDebugVertexCount + 3;
		indices[gRenderer->mDebugIndexCount + 4] = gRenderer->mDebugVertexCount + 3;
		indices[gRenderer->mDebugIndexCount + 5] = gRenderer->mDebugVertexCount + 2;
		indices[gRenderer->mDebugIndexCount + 6] = gRenderer->mDebugVertexCount + 2;
		indices[gRenderer->mDebugIndexCount + 7] = gRenderer->mDebugVertexCount + 0;

		indices[gRenderer->mDebugIndexCount + 8] = gRenderer->mDebugVertexCount + 4;
		indices[gRenderer->mDebugIndexCount + 9] = gRenderer->mDebugVertexCount + 5;
		indices[gRenderer->mDebugIndexCount + 10] = gRenderer->mDebugVertexCount + 5;
		indices[gRenderer->mDebugIndexCount + 11] = gRenderer->mDebugVertexCount + 7;
		indices[gRenderer->mDebugIndexCount + 12] = gRenderer->mDebugVertexCount + 7;
		indices[gRenderer->mDebugIndexCount + 13] = gRenderer->mDebugVertexCount + 6;
		indices[gRenderer->mDebugIndexCount + 14] = gRenderer->mDebugVertexCount + 6;
		indices[gRenderer->mDebugIndexCount + 15] = gRenderer->mDebugVertexCount + 4;

		indices[gRenderer->mDebugIndexCount + 16] = gRenderer->mDebugVertexCount + 0;
		indices[gRenderer->mDebugIndexCount + 17] = gRenderer->mDebugVertexCount + 4;

		indices[gRenderer->mDebugIndexCount + 18] = gRenderer->mDebugVertexCount + 1;
		indices[gRenderer->mDebugIndexCount + 19] = gRenderer->mDebugVertexCount + 5;

		indices[gRenderer->mDebugIndexCount + 20] = gRenderer->mDebugVertexCount + 2;
		indices[gRenderer->mDebugIndexCount + 21] = gRenderer->mDebugVertexCount + 6;

		indices[gRenderer->mDebugIndexCount + 22] = gRenderer->mDebugVertexCount + 3;
		indices[gRenderer->mDebugIndexCount + 23] = gRenderer->mDebugVertexCount + 7;

		gRenderer->mDebugVertexCount += 8;
		gRenderer->mDebugIndexCount += 24;
	}
}