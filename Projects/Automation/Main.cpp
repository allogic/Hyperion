#include <Engine/Scene.h>

#include <Engine/Common/Types.h>
#include <Engine/Common/Config.h>

#include <Engine/Ecs/Entity.h>

#include <Engine/Interface/FontAtlas.h>
#include <Engine/Interface/LayoutParser.h>
#include <Engine/Interface/Node.h>

#include <Engine/Vulkan/Window.h>
#include <Engine/Vulkan/Renderer.h>

#include <Automation/Scenes/Game.h>

using namespace hyperion;
using namespace automation;

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