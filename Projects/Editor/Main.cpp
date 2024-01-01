#include <Engine/Scene.h>

#include <Engine/Common/Types.h>
#include <Engine/Common/Config.h>

#include <Engine/Ecs/Entity.h>

#include <Engine/Ecs/Actors/Test.h>

#include <Engine/Interface/FontAtlas.h>
#include <Engine/Interface/LayoutParser.h>
#include <Engine/Interface/Node.h>

#include <Engine/Vulkan/Window.h>
#include <Engine/Vulkan/Renderer.h>

#include <Editor/Interface/MyButton.h>

using namespace hyperion;
using namespace editor;

I32 main()
{
	Window::Create("Editor", 1920, 1080);

	FontAtlas::Load(ROOT_PATH "Engine\\Fonts\\ProggyClean.ttf");
	FontAtlas::Load(ROOT_PATH "Engine\\Fonts\\Sagan.ttf");

	LayoutParser::Register<MyButton>();

	Node* layout = LayoutParser::Parse(ROOT_PATH "Projects\\Editor\\Interface\\Main.xml");

	Scene* scene = new Scene;

	Entity* root = scene->GetRoot();

	{
		U32 n = 3;

		for (U32 i = 0; i < n; ++i)
		{
			for (U32 j = 0; j < n; ++j)
			{
				Test* test = scene->CreateEntity<Test>("Test");

				Transform* transform = scene->GetTransform(test);
				transform->LocalPosition = R32V3{ i * 10.0F - (n * 10.0F / 2.0F) + 5.0F, 0.0F, j * 10.0F - (n * 10.0F / 2.0F) + 5.0F };
				transform->LocalEulerAngles = R32V3{ 0.0F, 0.0F, 0.0F };
				transform->LocalScale = R32V3{ 0.0F, 0.0F, 0.0F };

				for (U32 k = 0; k < 32; ++k)
				{
					test = scene->CreateEntity<Test>("Test", test);

					Transform* transform = scene->GetTransform(test);
					transform->LocalPosition = R32V3{ 0.0F, 2.0F, 0.0F };
					transform->LocalEulerAngles = R32V3{ 0.0F, 0.0F, 0.0F };
					transform->LocalScale = R32V3{ 0.0F, 0.0F, 0.0F };
					transform->WorldSpace = 0;
				}
			}
		}
	}

	root->PrintHierarchy();
	scene->PrintHierarchy();
	layout->PrintHierarchy();

	while (!Window::ShouldClose())
	{
		Window::MeasureTime();

		Renderer::DrawText(R32V3{ 100.0F, 400.0F, 0.5F }, "Sagan", "Hyperion Engine", 4.0F, 0xFFFFFFFF);
		Renderer::DrawText(R32V3{ 100.0F, 300.0F, 0.5F }, "ProggyClean", "- GPU Entity Component System", 2.0F, 0xFFFFFFFF);
		Renderer::DrawText(R32V3{ 100.0F, 250.0F, 0.5F }, "ProggyClean", "- Intermediate XML Interface", 2.0F, 0xFFFFFFFF);
		Renderer::DrawText(R32V3{ 100.0F, 200.0F, 0.5F }, "ProggyClean", "- Smart Font Texture Atlas", 2.0F, 0xFFFFFFFF);

		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1.0F, 0.0F, 0.0F }, 0xFF0000FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1.0F, 0.0F }, 0x00FF00FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1.0F }, 0x0000FFFF);

		Renderer::DrawDebugRectXZ(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 25.0F, 0.0F, 25.0F }, 0xFFFFFFFF);

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