#include <Engine/Common/Macros.h>

#include <Engine/Scene.h>

#include <Engine/Ecs/Actors/Player.h>

#include <Engine/Ecs/Components/Camera.h>

namespace hyperion
{
	Scene::Scene()
	{
		mRoot = CreateEntity<Entity>("Root");
		mPlayer = (Entity*)CreateEntity<Player>("Player");
	}

	Scene::~Scene()
	{
		delete mRoot;
	}

	void Scene::DestroyEntity(Entity* Entity)
	{
		// TODO
	}

	Transform* Scene::GetTransform(Entity* Entity)
	{
		return mTransformHierarchy.GetTransform(Entity->GetLevelIndex(), Entity->GetTransformIndex());
	}

	Camera* Scene::GetCamera(Entity* Entity)
	{
		if (!mCameras[Entity])
		{
			mCameras[Entity] = new Camera;
		}

		return mCameras[Entity];
	}

	void Scene::DispatchTransformHierarchy()
	{
		mTransformHierarchy.Dispatch();
	}

	void Scene::PrintHierarchy()
	{
		mTransformHierarchy.Print();
	}

	void Scene::Update()
	{
		mRoot->Update();
	}
}