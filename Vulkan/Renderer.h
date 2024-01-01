#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>

#include <Common/Types.h>
#include <Common/Config.h>

#include <Engine/Forward.h>

#include <Vulkan/Forward.h>
#include <Vulkan/Vertex.h>

#include <Vendor/vulkan/vulkan.h>

namespace vulkan
{
	using namespace common;
	using namespace engine;

#pragma pack (push, 1)
	struct TimeInfo
	{
		R32 Time;
		R32 DeltaTime;
	};

	struct ScreenInfo
	{
		R32V2 Size;
	};

	struct ViewProjection
	{
		R32M4 View;
		R32M4 Projection;
	};
#pragma pack (pop)

	struct CharacterInfo
	{
		R32V2 Size;
		R32V2 Bearing;
		U32 Advance;
	};

	struct FontInfo
	{
		U32 FontIndex;
		R32V2 FontSize;
		R32V2 CharacterSize;
		std::array<CharacterInfo, 128> CharacterInfos;
	};

	extern Renderer* gRenderer;

	class Renderer
	{
	public:

		inline auto GetTimeInfoBuffer() const { return mTimeInfoBuffer; }
		inline auto GetViewProjectionBuffer() const { return mViewProjectionBuffer; }
		inline auto GetComputeCommandBuffer() const { return mComputeCommandBuffer; }

	public:

		Renderer();
		virtual ~Renderer();

	private:

		void CreateCommandBuffer();
		void CreateSyncObjects();
		void CreateRenderPass();
		void CreateFrameBuffer();

	private:

#ifndef NDEBUG
		void CreateQueryPool();
#endif

	private:

		void DestroyCommandBuffer();
		void DestroySyncObjects();
		void DestroyRenderPass();
		void DestroyFrameBuffer();

	private:

		void DestroyDescriptorPool();
		void DestroyDescriptorSetLayout();
		void DestroyPipelineLayout();
		void DestroyPipeline();

	private:

#ifndef NDEBUG
		void DestroyQueryPool();
#endif

	public:

		void BuildTextDescriptorSets(U32 DescriptorCount);
		void BuildInterfaceDescriptorSets(U32 DescriptorCount);
		void BuildDebugDescriptorSets(U32 DescriptorCount);

		void UpdateTextDescriptorSets(U32 DescriptorIndex);
		void UpdateInterfaceDescriptorSets(U32 DescriptorIndex);
		void UpdateDebugDescriptorSets(U32 DescriptorIndex);

	private:

		void BuildGraphicCommandBuffer(Scene* Scene, U32 ImageIndex);
		void BuildComputeCommandBuffer(Scene* Scene);

	public:

		void ResizeBefore();
		void ResizeAfter();

	public:

		static void Render(Scene* Scene = 0);

	public:

		void UpdateFontAtlas(U32 Width, U32 Height, std::vector<U8> const& Atlas);
		void SetFontInfo(std::string const& Font, FontInfo const& Info);

	public:

		static R32 GetTextWidth(std::string const& Font, std::string const& Text, R32 Scale, R32* Value = 0);
		static void DrawText(R32V3 const& Position, std::string const& Font, std::string const& Text, R32 Scale, U32 Color, R32* Value = 0);

	public:

		static void DrawInterfaceRect(R32V3 const& Position, R32V2 const& Size, U32 Color);

	public:

		static void DrawDebugLine(R32V3 const& From, R32V3 const& To, U32 Color);
		static void DrawDebugRectXY(R32V3 const& Position, R32V3 const& Size, U32 Color, R32Q const& Rotation = { 0.0F, 0.0F, 0.0F, 1.0F });
		static void DrawDebugRectXZ(R32V3 const& Position, R32V3 const& Size, U32 Color, R32Q const& Rotation = { 0.0F, 0.0F, 0.0F, 1.0F });
		static void DrawDebugGridXZ(R32V3 const& Position, U32 DivisionX, U32 DivisionZ, R32 Size, U32 Color, R32Q const& Rotation = { 0.0F, 0.0F, 0.0F, 1.0F });
		static void DrawDebugBox(R32V3 const& Position, R32V3 const& Size, U32 Color, R32Q const& Rotation = { 0.0F, 0.0F, 0.0F, 1.0F });

	private:

		TimeInfo mTimeInfo = {};
		ScreenInfo mScreenInfo = {};
		ViewProjection mViewProjection = {};

		Buffer* mTimeInfoBuffer = 0;
		Buffer* mScreenInfoBuffer = 0;
		Buffer* mViewProjectionBuffer = 0;

	private:

		VkCommandBuffer mGraphicCommandBuffer = 0;
		VkCommandBuffer mComputeCommandBuffer = 0;

		VkSemaphore mGraphicCompleteSemaphore = 0;
		VkSemaphore mComputeCompleteSemaphore = 0;
		VkSemaphore mImageAvailableSemaphore = 0;

		VkFence mRenderFence = 0;

		VkRenderPass mRenderPass = 0;

		std::vector<VkFramebuffer> mFrameBuffers = {};

	private:

#ifndef NDEBUG
		VkQueryPool mQueryPool = 0;
#endif

	private:

		Buffer* mTextVertexBuffer = 0;
		Buffer* mTextIndexBuffer = 0;

		U32 mTextVertexCount = 0;
		U32 mTextIndexCount = 0;

		std::string mTextVertexFile = ROOT_PATH "Engine\\Shaders\\Text.vert.spv";
		std::string mTextFragmentFile = ROOT_PATH "Engine\\Shaders\\Text.frag.spv";

		std::vector<VkDescriptorSetLayoutBinding> mTextDescriptorSetLayoutBindings =
		{
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
		};

		std::vector<VkPushConstantRange> mTextPushConstantRanges =
		{

		};

		std::vector<VkVertexInputBindingDescription> mTextVertexInputBindingDescriptions =
		{
			{ TextVertexBindingId, sizeof(TextVertex), VK_VERTEX_INPUT_RATE_VERTEX },
		};

		std::vector<VkVertexInputAttributeDescription> mTextVertexInputAttributeDescriptions =
		{
			{ 0, TextVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, TextVertexBindingId, VK_FORMAT_R32G32_SFLOAT, sizeof(R32V3) },
			{ 2, TextVertexBindingId, VK_FORMAT_R32_UINT, sizeof(R32V3) + sizeof(R32V2) },
		};

		VkDescriptorPool mTextDescriptorPool = 0;
		VkDescriptorSetLayout mTextDescriptorSetLayout = 0;

		std::vector<VkDescriptorSet> mTextDescriptorSets = {};

		VkPipelineLayout mTextPipelineLayout = 0;
		VkPipeline mTextPipeline = 0;

		std::map<std::string, FontInfo> mFontInfos = {};

		Image* mFontAtlas = 0;

	private:

		Buffer* mInterfaceVertexBuffer = 0;
		Buffer* mInterfaceIndexBuffer = 0;

		U32 mInterfaceVertexCount = 0;
		U32 mInterfaceIndexCount = 0;

		std::string mInterfaceVertexFile = ROOT_PATH "Engine\\Shaders\\Interface.vert.spv";
		std::string mInterfaceFragmentFile = ROOT_PATH "Engine\\Shaders\\Interface.frag.spv";

		std::vector<VkDescriptorSetLayoutBinding> mInterfaceDescriptorSetLayoutBindings =
		{
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
		};

		std::vector<VkPushConstantRange> mInterfacePushConstantRanges =
		{

		};

		std::vector<VkVertexInputBindingDescription> mInterfaceVertexInputBindingDescriptions =
		{
			{ InterfaceVertexBindingId, sizeof(InterfaceVertex), VK_VERTEX_INPUT_RATE_VERTEX },
		};

		std::vector<VkVertexInputAttributeDescription> mInterfaceVertexInputAttributeDescriptions =
		{
			{ 0, InterfaceVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, InterfaceVertexBindingId, VK_FORMAT_R32G32_SFLOAT, sizeof(R32V3) },
			{ 2, InterfaceVertexBindingId, VK_FORMAT_R32_UINT, sizeof(R32V3) + sizeof(R32V2) },
		};

		VkDescriptorPool mInterfaceDescriptorPool = 0;
		VkDescriptorSetLayout mInterfaceDescriptorSetLayout = 0;

		std::vector<VkDescriptorSet> mInterfaceDescriptorSets = {};

		VkPipelineLayout mInterfacePipelineLayout = 0;
		VkPipeline mInterfacePipeline = 0;

	private:

		Buffer* mDebugVertexBuffer = 0;
		Buffer* mDebugIndexBuffer = 0;

		U32 mDebugVertexCount = 0;
		U32 mDebugIndexCount = 0;

		std::string mDebugVertexFile = ROOT_PATH "Engine\\Shaders\\Debug.vert.spv";
		std::string mDebugFragmentFile = ROOT_PATH "Engine\\Shaders\\Debug.frag.spv";

		std::vector<VkDescriptorSetLayoutBinding> mDebugDescriptorSetLayoutBindings =
		{
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
		};

		std::vector<VkPushConstantRange> mDebugPushConstantRanges =
		{

		};

		std::vector<VkVertexInputBindingDescription> mDebugVertexInputBindingDescriptions =
		{
			{ DebugVertexBindingId, sizeof(DebugVertex), VK_VERTEX_INPUT_RATE_VERTEX },
		};

		std::vector<VkVertexInputAttributeDescription> mDebugVertexInputAttributeDescriptions =
		{
			{ 0, DebugVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, DebugVertexBindingId, VK_FORMAT_R32_UINT, sizeof(R32V3) },
		};

		VkDescriptorPool mDebugDescriptorPool = 0;
		VkDescriptorSetLayout mDebugDescriptorSetLayout = 0;

		std::vector<VkDescriptorSet> mDebugDescriptorSets = {};

		VkPipelineLayout mDebugPipelineLayout = 0;
		VkPipeline mDebugPipeline = 0;
	};
}