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

#include <Automation/Scenes/Game.h>

using namespace automation;
using namespace common;
using namespace engine;
using namespace interface;
using namespace vulkan;

I32 main()
{
	Window::Create("Automation", 1920, 1080);

	FontAtlas::Load(ROOT_PATH "Engine\\Fonts\\ProggyClean.ttf");

	Node* layout = LayoutParser::Parse(ROOT_PATH "Projects\\Automation\\Interface\\Profiler.xml");

	Game* scene = new Game{ 5, 5 };

	Entity* root = scene->GetRoot();

	root->PrintHierarchy();
	scene->PrintHierarchy();
	layout->PrintHierarchy();

	while (!Window::ShouldClose())
	{
		Window::MeasureTime();

		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1.0F, 0.0F, 0.0F }, 0xFF0000FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1.0F, 0.0F }, 0x00FF00FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1.0F }, 0x0000FFFF);

		layout->Update();
		layout->Render();

		scene->Update();

		Renderer::Render(scene);

		Window::PollEvents();
	}

	delete scene;
	delete layout;

	Window::Destroy();

	return 0;
}