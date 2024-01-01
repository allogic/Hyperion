#include <Common/Macros.h>
#include <Common/Types.h>
#include <Common/Config.h>

#include <Engine/Scene.h>
#include <Engine/Entity.h>
#include <Engine/FontAtlas.h>

#include <Engine/Actors/Test.h>

#include <Interface/LayoutParser.h>
#include <Interface/Node.h>

#include <Vulkan/Window.h>
#include <Vulkan/Renderer.h>

using namespace common;
using namespace engine;
using namespace interface;
using namespace vulkan;

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