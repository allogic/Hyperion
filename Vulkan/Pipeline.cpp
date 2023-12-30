#include <Common/Macros.h>
#include <Common/FileSystem.h>

#include <Vulkan/Window.h>
#include <Vulkan/Pipeline.h>

namespace vulkan
{
	VkDescriptorPool Pipeline::CreateDescriptorPool(U32 PoolCount, std::vector<VkDescriptorSetLayoutBinding> const& DescriptorSetLayoutBindings)
	{
		VkDescriptorPool descriptorPool = 0;

		std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {};
		std::vector<U32> descriptorTypeCounts = {};

		descriptorTypeCounts.resize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);

		for (U32 i = 0; i < (U32)DescriptorSetLayoutBindings.size(); ++i)
		{
			descriptorTypeCounts[DescriptorSetLayoutBindings[i].descriptorType] += 1;
		}

		for (U32 i = 0; i < (U32)descriptorTypeCounts.size(); ++i)
		{
			if (descriptorTypeCounts[i] > 0)
			{
				VkDescriptorPoolSize descriptorPoolSize = {};
				descriptorPoolSize.type = (VkDescriptorType)i;
				descriptorPoolSize.descriptorCount = descriptorTypeCounts[i];

				descriptorPoolSizes.emplace_back(descriptorPoolSize);
			}
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolCreateInfo.poolSizeCount = (U32)descriptorPoolSizes.size();
		descriptorPoolCreateInfo.maxSets = PoolCount;

		VK_CHECK(vkCreateDescriptorPool(gWindow->GetDevice(), &descriptorPoolCreateInfo, 0, &descriptorPool));

		return descriptorPool;
	}

	VkDescriptorSetLayout Pipeline::CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> const& DescriptorSetLayoutBindings)
	{
		VkDescriptorSetLayout descriptorSetLayout = 0;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pBindings = DescriptorSetLayoutBindings.data();
		descriptorSetLayoutCreateInfo.bindingCount = (U32)DescriptorSetLayoutBindings.size();

		VK_CHECK(vkCreateDescriptorSetLayout(gWindow->GetDevice(), &descriptorSetLayoutCreateInfo, 0, &descriptorSetLayout));

		return descriptorSetLayout;
	}

	VkPipelineLayout Pipeline::CreatePipelineLayout(VkDescriptorSetLayout DescriptorSetLayout, std::vector<VkPushConstantRange> const& PushConstantRanges)
	{
		VkPipelineLayout pipelineLayout = 0;

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;
		pipelineLayoutCreateInfo.pPushConstantRanges = PushConstantRanges.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = (U32)PushConstantRanges.size();

		VK_CHECK(vkCreatePipelineLayout(gWindow->GetDevice(), &pipelineLayoutCreateInfo, 0, &pipelineLayout));

		return pipelineLayout;
	}

	VkPipeline Pipeline::CreateGraphicPipeline(VkPipelineLayout PipelineLayout, VkRenderPass RenderPass, VkPrimitiveTopology Topology, std::string const& VertexFile, std::string const& FragmentFile, std::vector<VkVertexInputBindingDescription> const& VertexInputBindingDescriptions, std::vector<VkVertexInputAttributeDescription> const& VertexInputAttributeDescriptions)
	{
		VkPipeline pipeline = 0;

		std::vector<U8> vertexBytes = FileSystem::ReadBinary(VertexFile);
		std::vector<U8> fragmentBytes = FileSystem::ReadBinary(FragmentFile);

		VkShaderModule vertexModule = 0;
		VkShaderModule fragmentModule = 0;

		{
			VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.codeSize = vertexBytes.size();
			shaderModuleCreateInfo.pCode = (U32 const*)vertexBytes.data();

			VK_CHECK(vkCreateShaderModule(gWindow->GetDevice(), &shaderModuleCreateInfo, 0, &vertexModule));
		}

		{
			VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.codeSize = fragmentBytes.size();
			shaderModuleCreateInfo.pCode = (U32 const*)fragmentBytes.data();

			VK_CHECK(vkCreateShaderModule(gWindow->GetDevice(), &shaderModuleCreateInfo, 0, &fragmentModule));
		}

		VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {};
		vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageCreateInfo.module = vertexModule;
		vertShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {};
		fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfo.module = fragmentModule;
		fragShaderStageCreateInfo.pName = "main";

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
		vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputCreateInfo.pVertexBindingDescriptions = VertexInputBindingDescriptions.data();
		vertexInputCreateInfo.vertexBindingDescriptionCount = (U32)VertexInputBindingDescriptions.size();
		vertexInputCreateInfo.pVertexAttributeDescriptions = VertexInputAttributeDescriptions.data();
		vertexInputCreateInfo.vertexAttributeDescriptionCount = (U32)VertexInputAttributeDescriptions.size();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
		inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyCreateInfo.topology = Topology;
		inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

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

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
		rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCreateInfo.depthClampEnable = VK_FALSE;
		rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerCreateInfo.lineWidth = 1.0F;
		rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizerCreateInfo.depthBiasConstantFactor = 0.0F;
		rasterizerCreateInfo.depthBiasClamp = 0.0F;
		rasterizerCreateInfo.depthBiasSlopeFactor = 0.0F;

		VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {};
		multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
		multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingCreateInfo.minSampleShading = 1.0F;
		multisamplingCreateInfo.pSampleMask = 0;
		multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
		colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendCreateInfo.attachmentCount = 1;
		colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
		colorBlendCreateInfo.blendConstants[0] = 0.0F;
		colorBlendCreateInfo.blendConstants[1] = 0.0F;
		colorBlendCreateInfo.blendConstants[2] = 0.0F;
		colorBlendCreateInfo.blendConstants[3] = 0.0F;

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
		dynamicStateCreateInfo.dynamicStateCount = (U32)dynamicStates.size();

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pStages = shaderStages.data();
		pipelineCreateInfo.stageCount = (U32)shaderStages.size();
		pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
		pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.layout = PipelineLayout;
		pipelineCreateInfo.renderPass = RenderPass;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = 0;

		VK_CHECK(vkCreateGraphicsPipelines(gWindow->GetDevice(), 0, 1, &pipelineCreateInfo, 0, &pipeline));

		vkDestroyShaderModule(gWindow->GetDevice(), vertexModule, 0);
		vkDestroyShaderModule(gWindow->GetDevice(), fragmentModule, 0);

		return pipeline;
	}

	VkPipeline Pipeline::CreateComputePipeline(VkPipelineLayout PipelineLayout, std::string const& ComputeFile)
	{
		VkPipeline pipeline = 0;

		std::vector<U8> computeBytes = FileSystem::ReadBinary(ComputeFile);

		VkShaderModule computeModule = 0;

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = computeBytes.size();
		shaderModuleCreateInfo.pCode = (U32 const*)computeBytes.data();

		VK_CHECK(vkCreateShaderModule(gWindow->GetDevice(), &shaderModuleCreateInfo, 0, &computeModule));

		VkPipelineShaderStageCreateInfo compShaderStageCreateInfo = {};
		compShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		compShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		compShaderStageCreateInfo.module = computeModule;
		compShaderStageCreateInfo.pName = "main";

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.layout = PipelineLayout;
		pipelineCreateInfo.stage = compShaderStageCreateInfo;

		VK_CHECK(vkCreateComputePipelines(gWindow->GetDevice(), 0, 1, &pipelineCreateInfo, 0, &pipeline));

		vkDestroyShaderModule(gWindow->GetDevice(), computeModule, 0);

		return pipeline;
	}
}