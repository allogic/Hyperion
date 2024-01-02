#include <Engine/Scene.h>

#include <Engine/Common/Types.h>
#include <Engine/Common/Config.h>

#include <Engine/Interface/Interface.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Renderer.h>

using namespace hyperion;

I32 main()
{
	Window::Create("Empty", 1920, 1080);

	Interface::LoadFont(ROOT_PATH "Engine\\Fonts\\ProggyClean.ttf");

#ifdef NDEBUG
	Interface::ParseLayout(ROOT_PATH "Projects\\Automation\\Interface\\Profiler.xml");
#endif

	while (!Window::ShouldClose())
	{
		Window::MeasureTime();

		Interface::Update();
		Interface::Render();

		Renderer::Render();

		Window::PollEvents();
	}

	Window::Destroy();

	return 0;
}