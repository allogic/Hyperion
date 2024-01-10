#include <Engine/Scene.h>

#include <Engine/Animation/Skeleton.h>

#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Entities/FpsPlayerEntity.h>

#include <Engine/Ecs/Components/AnimatorComponent.h>
#include <Engine/Ecs/Components/CameraComponent.h>
#include <Engine/Ecs/Components/RenderComponent.h>

#include <Engine/Renderer/Mesh.h>
#include <Engine/Renderer/Model.h>
#include <Engine/Renderer/Renderer.h>

namespace hyperion
{
	Scene::Scene()
	{
		mRootEntity = CreateEntity<Entity>("Root");
		mPlayerEntity = (Entity*)CreateEntity<FpsPlayerEntity>("Player");
	}

	Scene::~Scene()
	{
		delete mRootEntity;
	}

	bool Scene::Load(std::filesystem::path const& File)
	{
		return false;
	}

	bool Scene::Safe(std::filesystem::path const& File)
	{
		return false;
	}

	Entity* Scene::CreateEntityFromModel(Model* Model)
	{
		Entity* modelEntity = CreateEntity<Entity>(Model->GetName());

		Skeleton* skeleton = Model->GetSkeleton();

		AnimatorComponent* animatorComponent = modelEntity->AttachComponent<AnimatorComponent>();

		animatorComponent->SetSharedSkeleton(skeleton);

		Entity* meshEntity = modelEntity;

		for (auto const& [name, mesh] : Model->GetMeshes())
		{
			meshEntity = CreateEntity<Entity>(name, meshEntity);

			RenderComponent* renderComponent = meshEntity->AttachComponent<RenderComponent>();

			renderComponent->SetSharedMaterial(mesh->GetSharedMaterial());
			renderComponent->SetSharedBoneBuffer(skeleton->GetBuffer());
			renderComponent->SetSharedVertexBuffer(mesh->GetVertexBuffer());
			renderComponent->SetSharedIndexBuffer(mesh->GetIndexBuffer());
		}

		return modelEntity;
	}

	void Scene::DestroyEntity(Entity* Entity)
	{
		// TODO
	}

	void Scene::DispatchTransformHierarchy()
	{
		mTransformHierarchy.Dispatch();
	}

	void Scene::PrintHierarchy()
	{
		mRootEntity->PrintHierarchy();
	}

	void Scene::Commit()
	{
		mEntitiesToBeRendered.clear();
		mEntitiesToBeComputed.clear();
		mEntitiesToBeAnimated.clear();

		for (auto const& entity : mEntities)
		{
			AnimatorComponent* animatorComponent = entity->GetComponent<AnimatorComponent>();
			RenderComponent* renderComponent = entity->GetComponent<RenderComponent>();

			if (animatorComponent)
			{
				mEntitiesToBeAnimated.emplace_back(entity);
			}

			if (renderComponent)
			{
				mEntitiesToBeRendered.emplace_back(entity);
			}
		}

		U32 entitiesToBeRenderedCount = (U32)mEntitiesToBeRendered.size();

		if (entitiesToBeRenderedCount > 0)
		{
			gRenderer->BuildPhysicallBasedDescriptorSets(entitiesToBeRenderedCount);
		}

		for (U32 i = 0; i < entitiesToBeRenderedCount; ++i)
		{
			gRenderer->UpdatePhysicallyBasedDescriptorSets(i, mEntitiesToBeRendered[i]);
		}
	}

	void Scene::Update()
	{
		mRootEntity->Update();

		for (auto const& entity : mEntitiesToBeAnimated)
		{
			AnimatorComponent* animatorComponent = entity->GetComponent<AnimatorComponent>();

			animatorComponent->Update();
		}
	}
}