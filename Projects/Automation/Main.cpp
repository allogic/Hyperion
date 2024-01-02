#include <Engine/Scene.h>

#include <Engine/Common/Types.h>
#include <Engine/Common/Config.h>

#include <Engine/Ecs/Entity.h>

#include <Engine/Interface/Interface.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Renderer.h>

#include <Automation/Scenes/Game.h>

using namespace hyperion;
using namespace automation;

I32 main()
{
	Window::Create("Automation", 1920, 1080);

	Interface::LoadFont(ROOT_PATH "Engine\\Fonts\\ProggyClean.ttf");

#ifdef NDEBUG
	Interface::ParseLayout(ROOT_PATH "Projects\\Automation\\Interface\\Profiler.xml");
#endif

	Game* scene = new Game{ 5, 5 };

	Entity* root = scene->GetRoot();

	root->PrintHierarchy();
	scene->PrintHierarchy();

	while (!Window::ShouldClose())
	{
		Window::MeasureTime();

		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1.0F, 0.0F, 0.0F }, 0xFF0000FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1.0F, 0.0F }, 0x00FF00FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1.0F }, 0x0000FFFF);

		Interface::Update();
		Interface::Render();

		scene->Update();

		Renderer::Render(scene);

		Window::PollEvents();
	}

	delete scene;

	Interface::Destroy();
	Window::Destroy();

	return 0;
}