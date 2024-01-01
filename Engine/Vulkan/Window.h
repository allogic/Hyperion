#pragma once

#include <string>
#include <array>
#include <vector>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>
#include <Engine/Common/Timer.h>

#include <vulkan/vulkan.h>

namespace hyperion
{
	namespace KeyCode
	{
		enum Type
		{
			Key0 = 0x30,
			Key1 = 0x31,
			Key2 = 0x32,
			Key3 = 0x33,
			Key4 = 0x34,
			Key5 = 0x35,
			Key6 = 0x36,
			Key7 = 0x37,
			Key8 = 0x38,
			Key9 = 0x39,

			KeyA = 0x41,
			KeyB = 0x42,
			KeyC = 0x43,
			KeyD = 0x44,
			KeyE = 0x45,
			KeyF = 0x46,
			KeyG = 0x47,
			KeyH = 0x48,
			KeyI = 0x49,
			KeyJ = 0x4A,
			KeyK = 0x4B,
			KeyL = 0x4C,
			KeyM = 0x4D,
			KeyN = 0x4E,
			KeyO = 0x4F,
			KeyP = 0x50,
			KeyQ = 0x51,
			KeyR = 0x52,
			KeyS = 0x53,
			KeyT = 0x54,
			KeyU = 0x55,
			KeyV = 0x56,
			KeyW = 0x57,
			KeyX = 0x58,
			KeyY = 0x59,
			KeyZ = 0x5A,

			LeftShift = 0xA0,
			LeftCtrl = 0xA2,
		};
	}

	extern Window* gWindow;

	class Window
	{
	public:

		static inline auto GetTime() { return gWindow->mTime; }
		static inline auto GetDeltaTime() { return gWindow->mDeltaTime; }
		static inline auto GetWindowWidth() { return gWindow->mSwapchainExtent.width; }
		static inline auto GetWindowHeight() { return gWindow->mSwapchainExtent.height; }
		static inline auto GetWindowSize() { return R32V2{ gWindow->mSwapchainExtent.width, gWindow->mSwapchainExtent.height }; }
		static inline auto GetAspectRatio() { return (R32)gWindow->mSwapchainExtent.width / gWindow->mSwapchainExtent.height; }
		static inline auto GetMousePositionX() { return gWindow->mMouseX; }
		static inline auto GetMousePositionY() { return gWindow->mMouseY; }
		static inline auto GetMousePosition() { return R32V2{ gWindow->mMouseX, gWindow->mMouseY }; }

	public:

		inline auto GetSurface() const { return mSurface; }
		inline auto GetPreferedSurfaceFormat() const { return mPreferedSurfaceFormat; }
		inline auto GetPreferedPresentMode() const { return mPreferedPresentMode; }
		inline auto GetPhysicalDevice() const { return mPhysicalDevice; }
		inline auto const& GetPhysicalDeviceProperties() const { return mPhysicalDeviceProperties; }
		inline auto const& GetPhysicalDeviceFeatures() const { return mPhysicalDeviceFeatures; }
		inline auto GetDevice() const { return mDevice; }
		inline auto GetGraphicAndComputeQueueIndex() const { return mGraphicAndComputeQueueIndex; }
		inline auto GetPresentQueueIndex() const { return mPresentQueueIndex; }
		inline auto GetGraphicQueue() const { return mGraphicQueue; }
		inline auto GetComputeQueue() const { return mComputeQueue; }
		inline auto GetPresentQueue() const { return mPresentQueue; }
		inline auto GetCommandPool() const { return mCommandPool; }
		inline auto GetSwapChain() const { return mSwapChain; }
		inline auto GetSwapChainImageCount() const { return mSwapChainImageCount; }
		inline auto GetSwapChainImageView(U32 Index) const { return mSwapChainImageViews[Index]; }
		inline auto GetDepthImage() const { return mDepthImage; }
		inline auto GetDepthFormat() const { return mDepthFormat; }

	public:

		inline void SetMousePosition(R32 X, R32 Y) { mMouseX = X; mMouseY = Y; }

	public:

		static void Create(std::string const& Title, U32 Width, U32 Height);
		static void Destroy();
		static void MeasureTime();
		static bool ShouldClose();
		static void PollEvents();

	public:

		static bool KeyDown(KeyCode::Type Key);
		static bool KeyHeld(KeyCode::Type Key);
		static bool KeyUp(KeyCode::Type Key);

		static bool LeftMouseDown();
		static bool LeftMouseHeld();
		static bool LeftMouseUp();

		static bool MidMouseDown();
		static bool MidMouseHeld();
		static bool MidMouseUp();

		static bool RightMouseDown();
		static bool RightMouseHeld();
		static bool RightMouseUp();

	private:

		Window(std::string const& Title, U32 Width, U32 Height);
		virtual ~Window();

	private:

		void CreateNativeWindow(std::string const& Title, U32 Width, U32 Height);
		void CreateInstance();
		void CreateSurface();
		void FindPhysicalDevice();
		void FindQueueFamilies();
		void CheckPhysicalDeviceExtensions();
		void CreateDevice();
		void CheckSurfaceCapabilities();
		void CreateCommandPool();
		void CreateSwapChain();
		void CreateImages();
		void CreateImageViews();

	private:

		void DestroyInstance();
		void DestroySurface();
		void DestroyDevice();
		void DestroyCommandPool();
		void DestroySwapChain();
		void DestroyImages();
		void DestroyImageViews();

	public:

		void Resize();

	public:

		void DeviceWaitIdle() const;
		void GraphicQueueWaitIdle() const;
		void ComputeQueueWaitIdle() const;
		void PresentQueueWaitIdle() const;

	public:

		I32 CheckMemoryType(U32 TypeFilter, VkMemoryPropertyFlags MemoryPropertyFlags) const;

	private:

		VkFormat FindSupportedFormat(std::vector<VkFormat> const& Formats, VkImageTiling Tiling, VkFormatFeatureFlags Features);
		VkFormat FindDepthFormat();

	private:

#ifndef NDEBUG
		static VkResult CreateDebugUtilsMessengerEXT(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT* DebugCreateInfo, const VkAllocationCallbacks* Allocator, VkDebugUtilsMessengerEXT* Messenger);
		static void DestroyDebugUtilsMessengerEXT(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks* Allocator);
		static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData);
#endif

	private:

		Timer mTimer = {};
		Timer mDeltaTimer = {};

		R32 mTime = 0.0F;
		R32 mDeltaTime = 0.0F;

		U32 mTargetFps = 60;
		U32 mFps = 0;

		R32 mTargetFrameTime = 1.0F / mTargetFps;
		R32 mPrevFrameTime = 0.0F;

	private:

		R32 mMouseX = 0;
		R32 mMouseY = 0;

	private:

		VkInstance mInstance = 0;

#ifndef NDEBUG
		VkDebugUtilsMessengerEXT mDebugMessenger = 0;
#endif

	private:

		VkSurfaceKHR mSurface = 0;
		VkSurfaceCapabilitiesKHR mSurfaceCapabilities = {};

	private:

		VkExtent2D mSwapchainExtent = {};

		VkSwapchainKHR mSwapChain = 0;

		U32 mSwapChainImageCount = 0;

		std::vector<VkImage> mSwapChainImages = {};
		std::vector<VkImageView> mSwapChainImageViews = {};

	private:

		VkFormat mDepthFormat = VK_FORMAT_UNDEFINED;

		Image* mDepthImage = 0;

	private:

		VkSurfaceFormatKHR mPreferedSurfaceFormat = {};
		VkPresentModeKHR mPreferedPresentMode = {};

	private:

		VkPhysicalDeviceProperties mPhysicalDeviceProperties = {};
		VkPhysicalDeviceFeatures mPhysicalDeviceFeatures = {};

		VkPhysicalDevice mPhysicalDevice = 0;
		VkDevice mDevice = 0;

	private:

		I32 mGraphicAndComputeQueueIndex = -1;
		I32 mPresentQueueIndex = -1;

		VkQueue mGraphicQueue = 0;
		VkQueue mComputeQueue = 0;
		VkQueue mPresentQueue = 0;

		VkCommandPool mCommandPool = 0;
	};
}