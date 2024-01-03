#include <Engine/Common/Macros.h>
#include <Engine/Common/ValueDatabase.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Renderer.h>

#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/ImageVariance.h>

#include <windows.h>

#include <vulkan/vulkan_win32.h>

namespace hyperion
{
	Window* gWindow = 0;

	static HINSTANCE sModule = 0;
	static HWND sHwnd = 0;

	static bool sShouldClose = false;

	static std::vector<char const*> sLayerExtensions =
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
#ifdef _DEBUG
		"VK_EXT_debug_utils",
#endif
	};

	static std::vector<char const*> sDeviceExtensions =
	{
		"VK_KHR_swapchain",
	};

#ifdef _DEBUG
	static std::vector<char const*> sValidationLayers =
	{
		"VK_LAYER_KHRONOS_validation",
	};
#endif

	struct KeyState
	{
		U8 DownPrev : 1;
		U8 Down : 1;
		U8 Held : 1;
		U8 UpPrev : 1;
		U8 Up : 1;

		U8 Reserved : 3;
	};

	static std::array<KeyState, 0xFF> sKeyboardKeyStates = {};
	static std::array<KeyState, 0x3> sMouseKeyStates = {};

	static LRESULT WindowProcedure(HWND Handle, UINT Msg, WPARAM WParam, LPARAM LParam)
	{
		switch (Msg)
		{
			case WM_CLOSE:
			{				
				sShouldClose = true;

				break;
			}
			case WM_WINDOWPOSCHANGED:
			{
				WINDOWPOS* winPos = (WINDOWPOS*)LParam;

				if ((winPos->cx > 0) && (winPos->cy > 0))
				{
					gWindow->Resize();
				}

				break;
			}
			case WM_MOUSEMOVE:
			{
				RECT rect = {};

				GetClientRect(sHwnd, &rect);

				R32 w = (R32)rect.right - rect.left;
				R32 h = (R32)rect.bottom - rect.top;

				R32 x = (R32)LOWORD(LParam);
				R32 y = (R32)HIWORD(LParam);

				gWindow->SetMousePosition(x, h - y);

				break;
			}
			case WM_LBUTTONDOWN:
			{
				if (!sMouseKeyStates[0].DownPrev)
				{
					sMouseKeyStates[0].DownPrev = 0;
					sMouseKeyStates[0].Down = 1;
					sMouseKeyStates[0].Held = 0;
					sMouseKeyStates[0].UpPrev = 0;
					sMouseKeyStates[0].Up = 0;
				}

				break;
			}
			case WM_LBUTTONUP:
			{
				sMouseKeyStates[0].DownPrev = 0;
				sMouseKeyStates[0].Down = 0;
				sMouseKeyStates[0].Held = 0;
				sMouseKeyStates[0].UpPrev = 0;
				sMouseKeyStates[0].Up = 1;

				break;
			}
			case WM_MBUTTONDOWN:
			{
				if (!sMouseKeyStates[1].DownPrev)
				{
					sMouseKeyStates[1].DownPrev = 0;
					sMouseKeyStates[1].Down = 1;
					sMouseKeyStates[1].Held = 0;
					sMouseKeyStates[1].UpPrev = 0;
					sMouseKeyStates[1].Up = 0;
				}

				break;
			}
			case WM_MBUTTONUP:
			{
				sMouseKeyStates[1].DownPrev = 0;
				sMouseKeyStates[1].Down = 0;
				sMouseKeyStates[1].Held = 0;
				sMouseKeyStates[1].UpPrev = 0;
				sMouseKeyStates[1].Up = 1;

				break;
			}
			case WM_RBUTTONDOWN:
			{
				if (!sMouseKeyStates[2].DownPrev)
				{
					sMouseKeyStates[2].DownPrev = 0;
					sMouseKeyStates[2].Down = 1;
					sMouseKeyStates[2].Held = 0;
					sMouseKeyStates[2].UpPrev = 0;
					sMouseKeyStates[2].Up = 0;
				}

				break;
			}
			case WM_RBUTTONUP:
			{
				sMouseKeyStates[2].DownPrev = 0;
				sMouseKeyStates[2].Down = 0;
				sMouseKeyStates[2].Held = 0;
				sMouseKeyStates[2].UpPrev = 0;
				sMouseKeyStates[2].Up = 1;

				break;
			}
			case WM_KEYDOWN:
			{
				if (!sKeyboardKeyStates[WParam].DownPrev)
				{
					sKeyboardKeyStates[WParam].DownPrev = 0;
					sKeyboardKeyStates[WParam].Down = 1;
					sKeyboardKeyStates[WParam].Held = 0;
					sKeyboardKeyStates[WParam].UpPrev = 0;
					sKeyboardKeyStates[WParam].Up = 0;
				}

				break;
			}
			case WM_KEYUP:
			{
				sKeyboardKeyStates[WParam].DownPrev = 0;
				sKeyboardKeyStates[WParam].Down = 0;
				sKeyboardKeyStates[WParam].Held = 0;
				sKeyboardKeyStates[WParam].UpPrev = 0;
				sKeyboardKeyStates[WParam].Up = 1;

				break;
			}
			default:
			{
				return DefWindowProc(Handle, Msg, WParam, LParam);
			}
		}

		return 0;
	}

	void Window::Create(std::string const& Title, U32 Width, U32 Height)
	{
		gWindow = new Window{ Title, Width, Height };

		gWindow->CreateSwapChain();
		gWindow->CreateImages();
		gWindow->CreateImageViews();

		gRenderer = new Renderer;

		ShowWindow(sHwnd, SW_SHOW);
		UpdateWindow(sHwnd);
	}

	void Window::Destroy()
	{
		gWindow->DeviceWaitIdle();

		DestroyWindow(sHwnd);

		delete gRenderer;
		delete gWindow;
	}

	void Window::MeasureTime()
	{
		static bool init = true;

		if (init)
		{
			init = false;

			gWindow->mTimer.BeginMeasure();
			gWindow->mDeltaTimer.BeginMeasure();
		}

		gWindow->mTimer.EndMeasure();
		gWindow->mDeltaTimer.EndMeasure();

		gWindow->mTime = gWindow->mTimer.GetElapsedMicroSeconds<R32>();
		gWindow->mDeltaTime = gWindow->mDeltaTimer.GetElapsedMicroSeconds<R32>();

		gWindow->mFps++;

		if ((gWindow->mTime - gWindow->mPrevFrameTime) >= gWindow->mTargetFrameTime)
		{
			gWindow->mFps = 0;
			gWindow->mPrevFrameTime = gWindow->mTime;
		}

		ValueDatabase::SetValue("fps", (R32)gWindow->mFps);
		ValueDatabase::SetValue("time", gWindow->mTime);
		ValueDatabase::SetValue("deltaTime", gWindow->mDeltaTime);

		gWindow->mDeltaTimer.BeginMeasure();
	}

	bool Window::ShouldClose()
	{
		return sShouldClose;
	}

	void Window::PollEvents()
	{
		MSG msg = {};

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		for (U8 i = 0; i < 0xFF; ++i)
		{
			if (sKeyboardKeyStates[i].Down)
			{
				if (sKeyboardKeyStates[i].DownPrev)
				{
					sKeyboardKeyStates[i].Down = 0;
					sKeyboardKeyStates[i].Held = 1;
				}
				else
				{
					sKeyboardKeyStates[i].DownPrev = 1;
				}
			}

			if (sKeyboardKeyStates[i].Up)
			{
				sKeyboardKeyStates[i].Held = 0;

				if (sKeyboardKeyStates[i].UpPrev)
				{
					sKeyboardKeyStates[i].Up = 0;
				}
				else
				{
					sKeyboardKeyStates[i].UpPrev = 1;
				}
			}
		}

		for (U8 i = 0; i < 0x3; ++i)
		{
			if (sMouseKeyStates[i].Down)
			{
				if (sMouseKeyStates[i].DownPrev)
				{
					sMouseKeyStates[i].Down = 0;
					sMouseKeyStates[i].Held = 1;
				}
				else
				{
					sMouseKeyStates[i].DownPrev = 1;
				}
			}

			if (sMouseKeyStates[i].Up)
			{
				sMouseKeyStates[i].Held = 0;

				if (sMouseKeyStates[i].UpPrev)
				{
					sMouseKeyStates[i].Up = 0;
				}
				else
				{
					sMouseKeyStates[i].UpPrev = 1;
				}
			}
		}
	}

	bool Window::KeyDown(KeyCode::Type Key)
	{
		return sKeyboardKeyStates[Key].Down;
	}

	bool Window::KeyHeld(KeyCode::Type Key)
	{
		return sKeyboardKeyStates[Key].Held;
	}

	bool Window::KeyUp(KeyCode::Type Key)
	{
		return sKeyboardKeyStates[Key].Up;
	}

	bool Window::LeftMouseDown()
	{
		return sMouseKeyStates[0].Down;
	}

	bool Window::LeftMouseHeld()
	{
		return sMouseKeyStates[0].Held;
	}

	bool Window::LeftMouseUp()
	{
		return sMouseKeyStates[0].Up;
	}

	bool Window::MidMouseDown()
	{
		return sMouseKeyStates[1].Down;
	}

	bool Window::MidMouseHeld()
	{
		return sMouseKeyStates[1].Held;
	}

	bool Window::MidMouseUp()
	{
		return sMouseKeyStates[1].Up;
	}

	bool Window::RightMouseDown()
	{
		return sMouseKeyStates[2].Down;
	}

	bool Window::RightMouseHeld()
	{
		return sMouseKeyStates[2].Held;
	}

	bool Window::RightMouseUp()
	{
		return sMouseKeyStates[2].Up;
	}

	Window::Window(std::string const& Title, U32 Width, U32 Height)
	{
		CreateNativeWindow(Title, Width, Height);

		CreateInstance();
		CreateSurface();

		FindPhysicalDevice();
		FindQueueFamilies();

		CheckPhysicalDeviceExtensions();

		CreateDevice();

		CheckSurfaceCapabilities();

		CreateCommandPool();
	}

	Window::~Window()
	{
		DestroyImageViews();
		DestroyImages();
		DestroySwapChain();
		DestroyCommandPool();
		DestroyDevice();
		DestroySurface();
		DestroyInstance();
	}

	void Window::CreateNativeWindow(std::string const& Title, U32 Width, U32 Height)
	{
		sModule = GetModuleHandle(0);

		WNDCLASSEX windowClassEx = {};
		windowClassEx.cbSize = sizeof(windowClassEx);
		windowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClassEx.lpfnWndProc = WindowProcedure;
		windowClassEx.cbClsExtra = 0;
		windowClassEx.cbWndExtra = 0;
		windowClassEx.hInstance = sModule;
		windowClassEx.hIcon = LoadIcon(0, IDI_APPLICATION);
		windowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
		windowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClassEx.lpszMenuName = 0;
		windowClassEx.lpszClassName = Title.data();
		windowClassEx.hIconSm = LoadIcon(0, IDI_APPLICATION);

		RegisterClassEx(&windowClassEx);

		I32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
		I32 screenHeight = GetSystemMetrics(SM_CYSCREEN);
		I32 positionX = (screenWidth - Width) / 2;
		I32 positionY = (screenHeight - Height) / 2;

		sHwnd = CreateWindowEx(0, Title.data(), Title.data(), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, positionX, positionY, Width, Height, 0, 0, sModule, 0);

		LONG_PTR style = GetWindowLongPtr(sHwnd, GWL_STYLE);

		//SetWindowLongPtr(sHwnd, GWL_STYLE, style & ~(WS_CAPTION)); // TODO
	}

	void Window::CreateInstance()
	{
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "HyperionApplication";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "HyperionEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = (U32)sLayerExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = sLayerExtensions.data();

#ifdef _DEBUG
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;

		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		instanceCreateInfo.enabledLayerCount = (U32)sValidationLayers.size();
		instanceCreateInfo.ppEnabledLayerNames = sValidationLayers.data();
#endif

		VK_CHECK(vkCreateInstance(&instanceCreateInfo, 0, &mInstance));

#ifdef _DEBUG
		CreateDebugUtilsMessengerEXT(mInstance, &debugCreateInfo, 0, &mDebugMessenger);
#endif
	}

	void Window::CreateSurface()
	{
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hwnd = sHwnd;
		surfaceCreateInfo.hinstance = sModule;

		VK_CHECK(vkCreateWin32SurfaceKHR(mInstance, &surfaceCreateInfo, 0, &mSurface));
	}

	void Window::FindPhysicalDevice()
	{
		U32 physicalDeviceCount = 0;
		VK_CHECK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, 0));

		std::vector<VkPhysicalDevice> physicalDevices = {};
		physicalDevices.resize(physicalDeviceCount);

		VK_CHECK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, physicalDevices.data()));

		for (auto const& physicalDevice : physicalDevices)
		{
			vkGetPhysicalDeviceProperties(physicalDevice, &mPhysicalDeviceProperties);
			vkGetPhysicalDeviceFeatures(physicalDevice, &mPhysicalDeviceFeatures);

			if (mPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				if (mPhysicalDeviceFeatures.geometryShader && mPhysicalDeviceFeatures.samplerAnisotropy)
				{
					mPhysicalDevice = physicalDevice;

					break;
				}
			}
		}
	}

	void Window::FindQueueFamilies()
	{
		U32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, 0);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties = {};
		queueFamilyProperties.resize(queueFamilyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		for (U32 i = 0; i < queueFamilyCount; ++i)
		{
			U32 graphicSupport = 0;
			U32 computeSupport = 0;
			U32 presentSupport = 0;

			graphicSupport = queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
			computeSupport = queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;

			VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &presentSupport));

			if (graphicSupport && computeSupport && (mGraphicAndComputeQueueIndex == -1))
			{
				mGraphicAndComputeQueueIndex = i;
			}
			else if (presentSupport && (mPresentQueueIndex == -1))
			{
				mPresentQueueIndex = i;
			}

			if ((mGraphicAndComputeQueueIndex != -1) && (mPresentQueueIndex != -1))
			{
				break;
			}
		}

		LOG("Queue indices\n");
		LOG("  GraphicAndComputeQueueIndex %d\n", mGraphicAndComputeQueueIndex);
		LOG("  PresentQueueIndex %d\n", mPresentQueueIndex);
		LOG("\n");
	}

	void Window::CheckPhysicalDeviceExtensions()
	{
		U32 availableDeviceExtensionCount = 0;
		VK_CHECK(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, 0, &availableDeviceExtensionCount, 0));

		std::vector<VkExtensionProperties> availableDeviceExtensions = {};
		availableDeviceExtensions.resize(availableDeviceExtensionCount);

		VK_CHECK(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, 0, &availableDeviceExtensionCount, availableDeviceExtensions.data()));

		LOG("Device extensions\n");

		for (U32 i = 0; i < (U32)sDeviceExtensions.size(); ++i)
		{
			bool deviceExtensionsAvailable = false;

			for (U32 j = 0; j < availableDeviceExtensionCount; ++j)
			{
				if (std::strcmp(sDeviceExtensions[i], availableDeviceExtensions[j].extensionName) == 0)
				{
					LOG("  Found %s\n", sDeviceExtensions[i]);

					deviceExtensionsAvailable = true;

					break;
				}
			}

			if (!deviceExtensionsAvailable)
			{
				LOG("  Missing %s\n", sDeviceExtensions[i]);

				break;
			}
		}

		LOG("\n");
	}

	void Window::CreateDevice()
	{
		R32 queuePriority = 1.0F;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
		queueCreateInfos.resize(2);

		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].queueFamilyIndex = mGraphicAndComputeQueueIndex;
		queueCreateInfos[0].queueCount = 1;
		queueCreateInfos[0].pQueuePriorities = &queuePriority;

		queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[1].queueFamilyIndex = mPresentQueueIndex;
		queueCreateInfos[1].queueCount = 1;
		queueCreateInfos[1].pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
		physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = (U32)queueCreateInfos.size();
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
		deviceCreateInfo.ppEnabledExtensionNames = sDeviceExtensions.data();
		deviceCreateInfo.enabledExtensionCount = (U32)sDeviceExtensions.size();
#ifdef _DEBUG
		deviceCreateInfo.ppEnabledLayerNames = sValidationLayers.data();
		deviceCreateInfo.enabledLayerCount = (U32)sValidationLayers.size();
#endif

		VK_CHECK(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, 0, &mDevice));

		vkGetDeviceQueue(mDevice, mGraphicAndComputeQueueIndex, 0, &mGraphicQueue);
		vkGetDeviceQueue(mDevice, mGraphicAndComputeQueueIndex, 0, &mComputeQueue);
		vkGetDeviceQueue(mDevice, mPresentQueueIndex, 0, &mPresentQueue);
	}

	void Window::CheckSurfaceCapabilities()
	{
		U32 surfaceFormatCount = 0;
		U32 presentModeCount = 0;

		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatCount, 0));
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModeCount, 0));

		std::vector<VkSurfaceFormatKHR> surfaceFormats = {};
		std::vector<VkPresentModeKHR> presentModes = {};

		surfaceFormats.resize(surfaceFormatCount);
		presentModes.resize(presentModeCount);

		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &surfaceFormatCount, surfaceFormats.data()));
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModeCount, presentModes.data()));

		for (auto const& surfaceFormat : surfaceFormats)
		{
			if ((surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) && (surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			{
				mPreferedSurfaceFormat = surfaceFormat;

				break;
			}
		}

		for (auto const& presentMode : presentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				mPreferedPresentMode = presentMode;

				break;
			}
		}
	}

	void Window::CreateCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = mGraphicAndComputeQueueIndex;

		VK_CHECK(vkCreateCommandPool(mDevice, &commandPoolCreateInfo, 0, &mCommandPool));
	}

	void Window::CreateSwapChain()
	{
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &mSurfaceCapabilities));

		mSwapchainExtent = mSurfaceCapabilities.currentExtent;

		U32 nMinImageCount = mSurfaceCapabilities.minImageCount + 1;

		VkSwapchainCreateInfoKHR swapChaincreateInfo = {};
		swapChaincreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChaincreateInfo.surface = mSurface;
		swapChaincreateInfo.minImageCount = nMinImageCount;
		swapChaincreateInfo.imageFormat = mPreferedSurfaceFormat.format;
		swapChaincreateInfo.imageColorSpace = mPreferedSurfaceFormat.colorSpace;
		swapChaincreateInfo.imageExtent.width = mSwapchainExtent.width;
		swapChaincreateInfo.imageExtent.height = mSwapchainExtent.height;
		swapChaincreateInfo.imageArrayLayers = 1;
		swapChaincreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChaincreateInfo.preTransform = mSurfaceCapabilities.currentTransform;
		swapChaincreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChaincreateInfo.presentMode = mPreferedPresentMode;
		swapChaincreateInfo.clipped = VK_TRUE;
		swapChaincreateInfo.oldSwapchain = 0;

		std::vector<U32> queueFamilies = { (U32)mGraphicAndComputeQueueIndex, (U32)mPresentQueueIndex };

		if (mGraphicAndComputeQueueIndex == mPresentQueueIndex)
		{
			swapChaincreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChaincreateInfo.queueFamilyIndexCount = 0;
			swapChaincreateInfo.pQueueFamilyIndices = 0;
		}
		else
		{
			swapChaincreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChaincreateInfo.pQueueFamilyIndices = queueFamilies.data();
			swapChaincreateInfo.queueFamilyIndexCount = (U32)queueFamilies.size();
		}

		VK_CHECK(vkCreateSwapchainKHR(mDevice, &swapChaincreateInfo, 0, &mSwapChain));
	}

	void Window::CreateImages()
	{
		VK_CHECK(vkGetSwapchainImagesKHR(mDevice, mSwapChain, &mSwapChainImageCount, 0));

		mSwapChainImages.resize(mSwapChainImageCount);

		VK_CHECK(vkGetSwapchainImagesKHR(mDevice, mSwapChain, &mSwapChainImageCount, mSwapChainImages.data()));

		mDepthFormat = FindDepthFormat();

		mDepthImage = ImageVariance::CreateDepthImage(mSwapchainExtent.width, mSwapchainExtent.height, mDepthFormat);
	}

	void Window::CreateImageViews()
	{
		mSwapChainImageViews.resize(mSwapChainImageCount);

		for (U32 i = 0; i < mSwapChainImageCount; ++i)
		{
			mSwapChainImageViews[i] = Image::CreateImageView(mSwapChainImages[i], VK_IMAGE_VIEW_TYPE_2D, mPreferedSurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void Window::DestroyInstance()
	{
#ifdef _DEBUG
		DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, 0);
#endif

		vkDestroyInstance(mInstance, 0);
	}

	void Window::DestroySurface()
	{
		vkDestroySurfaceKHR(mInstance, mSurface, 0);
	}

	void Window::DestroyDevice()
	{
		vkDestroyDevice(mDevice, 0);
	}

	void Window::DestroyCommandPool()
	{
		vkDestroyCommandPool(mDevice, mCommandPool, 0);
	}

	void Window::DestroySwapChain()
	{
		vkDestroySwapchainKHR(mDevice, mSwapChain, 0);
	}

	void Window::DestroyImages()
	{
		delete mDepthImage;
	}

	void Window::DestroyImageViews()
	{
		for (U32 i = 0; i < mSwapChainImageCount; ++i)
		{
			Image::DestroyImageView(mSwapChainImageViews[i]);
		}
	}

	void Window::Resize()
	{
		gRenderer->ResizeBefore();

		DestroyImageViews();
		DestroyImages();
		DestroySwapChain();

		CreateSwapChain();
		CreateImages();
		CreateImageViews();

		gRenderer->ResizeAfter();
	}

	void Window::DeviceWaitIdle() const
	{
		VK_CHECK(vkDeviceWaitIdle(mDevice));
	}

	void Window::GraphicQueueWaitIdle() const
	{
		VK_CHECK(vkQueueWaitIdle(mGraphicQueue));
	}

	void Window::ComputeQueueWaitIdle() const
	{
		VK_CHECK(vkQueueWaitIdle(mComputeQueue));
	}

	void Window::PresentQueueWaitIdle() const
	{
		VK_CHECK(vkQueueWaitIdle(mPresentQueue));
	}

	I32 Window::CheckMemoryType(U32 TypeFilter, VkMemoryPropertyFlags MemoryPropertyFlags) const
	{
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &physicalDeviceMemoryProperties);

		for (U32 i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i)
		{
			if ((TypeFilter & (1 << i)) && ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & MemoryPropertyFlags) == MemoryPropertyFlags))
			{
				return i;
			}
		}

		return -1;
	}

	VkFormat Window::FindSupportedFormat(std::vector<VkFormat> const& Formats, VkImageTiling Tiling, VkFormatFeatureFlags Features)
	{
		for (auto const& format : Formats)
		{
			VkFormatProperties formatProperties = {};
			vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &formatProperties);

			if (Tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & Features) == Features)
			{
				return format;
			}
			else if (Tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & Features) == Features)
			{
				return format;
			}
		}

		return VK_FORMAT_UNDEFINED;
	}

	VkFormat Window::FindDepthFormat()
	{
		std::vector<VkFormat> formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };

		return FindSupportedFormat(formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

#ifdef _DEBUG
	VkResult Window::CreateDebugUtilsMessengerEXT(VkInstance Instance, const VkDebugUtilsMessengerCreateInfoEXT* DebugCreateInfo, const VkAllocationCallbacks* Allocator, VkDebugUtilsMessengerEXT* Messenger)
	{
		PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");

		if (createDebugUtilsMessengerEXT)
		{
			return createDebugUtilsMessengerEXT(Instance, DebugCreateInfo, Allocator, Messenger);
		}

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void Window::DestroyDebugUtilsMessengerEXT(VkInstance Instance, VkDebugUtilsMessengerEXT Messenger, const VkAllocationCallbacks* Allocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");

		if (destroyDebugUtilsMessengerEXT)
		{
			destroyDebugUtilsMessengerEXT(Instance, Messenger, Allocator);
		}
	}

	VkBool32 Window::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* CallbackData, void* UserData)
	{
		LOG("%s\n", CallbackData->pMessage);

		return VK_FALSE;
	}
#endif
}