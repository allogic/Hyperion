#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>

#include <Engine/Common/Types.h>
#include <Engine/Common/Config.h>

#include <Engine/Ecs/PerEntityData.h>

#include <Engine/Forward.h>

#include <Engine/Renderer/Vertex.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
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

	struct ProjectionInfo
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
		inline auto GetProjectionInfoBuffer() const { return mProjectionInfoBuffer; }
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

#ifdef _DEBUG
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

#ifdef _DEBUG
		void DestroyQueryPool();
#endif

	public:

		void BuildPhysicallBasedDescriptorSets(U32 DescriptorCount);
		void BuildTextDescriptorSets(U32 DescriptorCount);
		void BuildInterfaceDescriptorSets(U32 DescriptorCount);
		void BuildDebugDescriptorSets(U32 DescriptorCount);

		void UpdatePhysicallyBasedDescriptorSets(U32 DescriptorIndex, Scene* Scene, Entity* Entity);
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
		ProjectionInfo mProjectionInfo = {};

		Buffer* mTimeInfoBuffer = 0;
		Buffer* mScreenInfoBuffer = 0;
		Buffer* mProjectionInfoBuffer = 0;

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

#ifdef _DEBUG
		VkQueryPool mQueryPool = 0;
#endif

	private:

		std::string mPhysicallyBasedVertexFile = ROOT_PATH "Engine\\Shaders\\PhysicallyBased.vert.spv";
		std::string mPhysicallyBasedFragmentFile = ROOT_PATH "Engine\\Shaders\\PhysicallyBased.frag.spv";

		std::vector<VkDescriptorSetLayoutBinding> mPhysicallyBasedDescriptorSetLayoutBindings =
		{
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },

			{ 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
			{ 5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
			{ 6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
			{ 7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
			{ 8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
			{ 9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
		};

		std::vector<VkPushConstantRange> mPhysicallyBasedPushConstantRanges =
		{
			{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PerEntityData) },
		};

		std::vector<VkVertexInputBindingDescription> mPhysicallyBasedVertexInputBindingDescriptions =
		{
			{ PhysicallyBasedVertexBindingId, sizeof(PhysicallyBasedVertex), VK_VERTEX_INPUT_RATE_VERTEX },
		};

		std::vector<VkVertexInputAttributeDescription> mPhysicallyBasedVertexInputAttributeDescriptions =
		{
			{ 0, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, Normal) },
			{ 2, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, Tangent) },
			{ 3, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, BiTangent) },

			{ 4, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel0) },
			{ 5, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel1) },
			{ 6, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel2) },
			{ 7, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel3) },
			{ 8, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel4) },
			{ 9, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel5) },
			{ 10, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel6) },
			{ 11, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, ColorChannel7) },

			{ 12, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel0) },
			{ 13, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel1) },
			{ 14, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel2) },
			{ 15, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel3) },
			{ 16, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel4) },
			{ 17, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel5) },
			{ 18, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel6) },
			{ 19, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PhysicallyBasedVertex, TexCoordChannel7) },

			{ 20, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SINT, offsetof(PhysicallyBasedVertex, BoneIndices) },
			{ 21, PhysicallyBasedVertexBindingId, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PhysicallyBasedVertex, BoneWeights) },
		};

		VkDescriptorPool mPhysicallyBasedDescriptorPool = 0;
		VkDescriptorSetLayout mPhysicallyBasedDescriptorSetLayout = 0;

		std::vector<VkDescriptorSet> mPhysicallyBasedDescriptorSets = {};

		VkPipelineLayout mPhysicallyBasedPipelineLayout = 0;
		VkPipeline mPhysicallyBasedPipeline = 0;

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
			{ 1, TextVertexBindingId, VK_FORMAT_R32_UINT, offsetof(TextVertex, ColorChannel0) },
			{ 2, TextVertexBindingId, VK_FORMAT_R32G32_SFLOAT, offsetof(TextVertex, TexCoordChannel0) },
		};

		VkDescriptorPool mTextDescriptorPool = 0;
		VkDescriptorSetLayout mTextDescriptorSetLayout = 0;

		std::vector<VkDescriptorSet> mTextDescriptorSets = {};

		VkPipelineLayout mTextPipelineLayout = 0;
		VkPipeline mTextPipeline = 0;

		std::map<std::string, FontInfo> mFontInfos = {};

		Image* mFontAtlasImage = 0;

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
			{ 1, InterfaceVertexBindingId, VK_FORMAT_R32_UINT, offsetof(InterfaceVertex, ColorChannel0) },
			{ 2, InterfaceVertexBindingId, VK_FORMAT_R32G32_SFLOAT, offsetof(InterfaceVertex, TexCoordChannel0) },
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
			{ 1, DebugVertexBindingId, VK_FORMAT_R32_UINT, offsetof(DebugVertex, ColorChannel0) },
		};

		VkDescriptorPool mDebugDescriptorPool = 0;
		VkDescriptorSetLayout mDebugDescriptorSetLayout = 0;

		std::vector<VkDescriptorSet> mDebugDescriptorSets = {};

		VkPipelineLayout mDebugPipelineLayout = 0;
		VkPipeline mDebugPipeline = 0;
	};
}