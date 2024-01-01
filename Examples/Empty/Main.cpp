#include <Engine/Scene.h>

#include <Engine/Common/Types.h>
#include <Engine/Common/Config.h>

#include <Engine/Interface/FontAtlas.h>
#include <Engine/Interface/LayoutParser.h>
#include <Engine/Interface/Node.h>

#include <Engine/Vulkan/Window.h>
#include <Engine/Vulkan/Renderer.h>

using namespace hyperion;

I32 main()
{
	Window::Create("Empty", 1920, 1080);

	FontAtlas::Load(ROOT_PATH "Engine\\Fonts\\ProggyClean.ttf");

	Node* layout = LayoutParser::Parse(ROOT_PATH "Examples\\Empty\\Interface\\Profiler.xml");

	while (!Window::ShouldClose())
	{
		Window::MeasureTime();

		layout->Update();
		layout->Render();

		Renderer::Render();

		Window::PollEvents();
	}

	delete layout;

	Window::Destroy();

	return 0;
}