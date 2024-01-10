#include <Engine/Scene.h>

#include <Engine/Common/Config.h>
#include <Engine/Common/Macros.h>
#include <Engine/Common/Types.h>

#include <Engine/Ecs/Entity.h>

#include <Engine/Ecs/Components/AnimatorComponent.h>

#include <Engine/Ecs/Entities/TestEntity.h>

#include <Engine/Interface/Interface.h>

#include <Engine/Memory/FixedSizeAccessor.h>
#include <Engine/Memory/FixedSizeAllocator.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Model.h>
#include <Engine/Renderer/Renderer.h>

#include <Editor/Interface/MyButton.h>

using namespace hyperion;
using namespace editor;

I32 main()
{
	Window::Create("Editor", 1920, 1080);

	LayoutParser::Register<MyButton>();

	Interface::LoadFont(ROOT_PATH "Engine\\Fonts\\ProggyClean.ttf");
	Interface::LoadFont(ROOT_PATH "Engine\\Fonts\\Sagan.ttf");

#ifdef _DEBUG
	Interface::ParseLayout(ROOT_PATH "Projects\\Editor\\Interface\\Profiler.xml");
#endif

	Interface::ParseLayout(ROOT_PATH "Projects\\Editor\\Interface\\ActionBar.xml");

	Model* model = Model::Load(ROOT_PATH "Projects\\Editor\\Models\\Chaman\\Chaman.glb");

	auto animations = model->GetAnimations(); // TODO: Refactor this!

	model->PrintStats();

	Scene* scene = new Scene;

	Entity* modelEntity = scene->CreateEntityFromModel(model);

	//modelEntity->GetComponent<AnimatorComponent>()->Play(animations["Take 001"]);
	modelEntity->GetComponent<AnimatorComponent>()->Play(animations["Walk_Cycle"]);
	//modelEntity->GetComponent<AnimatorComponent>()->Play(animations["Attack"]);

	scene->Commit();

	{
		U32 n = 1;

		for (U32 i = 0; i < n; ++i)
		{
			for (U32 j = 0; j < n; ++j)
			{
				TestEntity* testEntity = scene->CreateEntity<TestEntity>("Test");

				Transform* transform = testEntity->GetTransform();
				transform->LocalPosition = R32V3{ i * 10.0F - (n * 10.0F / 2.0F) + 5.0F, 0.0F, j * 10.0F - (n * 10.0F / 2.0F) + 5.0F };
				transform->LocalEulerAngles = R32V3{ 0.0F, 0.0F, 0.0F };
				transform->LocalScale = R32V3{ 1.0F, 1.0F, 1.0F };

				for (U32 k = 0; k < 32; ++k)
				{
					testEntity = scene->CreateEntity<TestEntity>("Test", testEntity);

					Transform* transform = testEntity->GetTransform();
					transform->LocalPosition = R32V3{ 0.0F, 2.0F, 0.0F };
					transform->LocalEulerAngles = R32V3{ 0.0F, 0.0F, 0.0F };
					transform->LocalScale = R32V3{ 1.0F, 1.0F, 1.0F };
					transform->WorldSpace = 0;
				}
			}
		}
	}

	scene->PrintHierarchy();

	while (!Window::ShouldClose())
	{
		Window::MeasureTime();

		Renderer::DrawText(R32V3{ 100.0F, 400.0F, 0.5F }, "Sagan", "Hyperion Engine", 2.0F, 0xFFFFFFFF);
		Renderer::DrawText(R32V3{ 100.0F, 300.0F, 0.5F }, "ProggyClean", "- GPU Entity Component System", 2.0F, 0xFFFFFFFF);
		Renderer::DrawText(R32V3{ 100.0F, 250.0F, 0.5F }, "ProggyClean", "- Intermediate XML Interface", 2.0F, 0xFFFFFFFF);
		Renderer::DrawText(R32V3{ 100.0F, 200.0F, 0.5F }, "ProggyClean", "- Smart Font Texture Atlas", 2.0F, 0xFFFFFFFF);

		//Renderer::DrawDebugSkeleton(model->GetSkeleton());

		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 1.0F, 0.0F, 0.0F }, 0xFF0000FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 1.0F, 0.0F }, 0x00FF00FF);
		Renderer::DrawDebugLine(R32V3{ 0.0F, 0.0F, 0.0F }, R32V3{ 0.0F, 0.0F, 1.0F }, 0x0000FFFF);

		Renderer::DrawDebugGridXZ(R32V3{ 0.0F, 0.0F, 0.0F }, 10, 10, 1.0F, 0xFFFFFF44);

		Interface::Update();
		Interface::Render();

		scene->Update();

		Renderer::Render(scene);

		Window::PollEvents();
	}
	
	delete scene;
	delete model;

	Window::Destroy();

	return 0;
}