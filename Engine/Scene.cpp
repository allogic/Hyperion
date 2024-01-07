#include <Engine/Scene.h>

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
		// TODO: Refactor this!
		Buffer* boneTransformBuffer = BufferVariance::CreateUniformCoherent(sizeof(R32M4) * 100);

		R32M4* boneTransform = boneTransformBuffer->GetMappedData<R32M4>();

		for (U32 i = 0; i < 100; ++i) // TODO
		{
			*(boneTransform + i) = R32M4{ 1.0F };
		}

		Entity* modelEntity = CreateEntity<Entity>(Model->GetName());

		AnimatorComponent* animatorComponent = modelEntity->AttachComponent<AnimatorComponent>();

		animatorComponent->SetBoneTransformBuffer(boneTransformBuffer);
		animatorComponent->SetSharedSkeleton(Model->GetSkeleton());

		Entity* meshEntity = modelEntity;

		for (auto const& [name, mesh] : Model->GetMeshes())
		{
			meshEntity = CreateEntity<Entity>(name, meshEntity);

			RenderComponent* renderComponent = meshEntity->AttachComponent<RenderComponent>();

			renderComponent->SetSharedMaterial(mesh->GetSharedMaterial());
			renderComponent->SetSharedBoneTransformBuffer(boneTransformBuffer);
			renderComponent->SetSharedVertexBuffer(mesh->GetVertexBuffer());
			renderComponent->SetSharedIndexBuffer(mesh->GetIndexBuffer());
		}

		return modelEntity;
	}

	void Scene::DestroyEntity(Entity* Entity)
	{
		// TODO
	}

	Transform* Scene::GetTransform(Entity* Entity)
	{
		return mTransformHierarchy.GetTransform(Entity->GetLevelIndex(), Entity->GetTransformIndex());
	}

	CameraComponent* Scene::GetCameraComponent(Entity* Entity)
	{
		if (!mCameraComponents[Entity])
		{
			mCameraComponents[Entity] = new CameraComponent;
		}

		return mCameraComponents[Entity];
	}

	void Scene::DispatchTransformHierarchy()
	{
		mTransformHierarchy.Dispatch();
	}

	void Scene::PrintHierarchy()
	{
		mTransformHierarchy.Print();
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