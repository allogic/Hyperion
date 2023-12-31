#pragma once

#include <vector>
#include <string>

#include <Engine/Common/Types.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	class Pipeline
	{
	public:

		static VkDescriptorPool CreateDescriptorPool(U32 PoolCount, std::vector<VkDescriptorSetLayoutBinding> const& DescriptorSetLayoutBindings);
		static VkDescriptorSetLayout CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> const& DescriptorSetLayoutBindings);

		static VkPipelineLayout CreatePipelineLayout(VkDescriptorSetLayout DescriptorSetLayout, std::vector<VkPushConstantRange> const& PushConstantRanges);

		static VkPipeline CreateGraphicPipeline(VkPipelineLayout PipelineLayout, VkRenderPass RenderPass, VkPrimitiveTopology Topology, std::string const& VertexFile, std::string const& FragmentFile, std::vector<VkVertexInputBindingDescription> const& VertexInputBindingDescriptions, std::vector<VkVertexInputAttributeDescription> const& VertexInputAttributeDescriptions);
		static VkPipeline CreateComputePipeline(VkPipelineLayout PipelineLayout, std::string const& ComputeFile);
	};
}