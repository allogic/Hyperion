#pragma once

#include <string>
#include <vector>
#include <map>

#include <Common/Types.h>

#include <Engine/Forward.h>
#include <Engine/Entity.h>
#include <Engine/Transform.h>
#include <Engine/TransformHierarchy.h>

#include <Vulkan/Forward.h>
#include <Vulkan/Buffer.h>
#include <Vulkan/Renderer.h>

#include <Vendor/vulkan/vulkan.h>

namespace engine
{
	using namespace common;
	using namespace vulkan;

	class Scene
	{
	public:

		inline auto GetRoot() const { return mRoot; }
		inline auto const& GetEntities() const { return mEntities; }
		inline auto GetPlayer() const { return mPlayer; }

	public:

		Scene();
		virtual ~Scene();

	public:

		template<typename E, typename ... Argument>
		E* CreateEntity(std::string const& Name, Entity* Parent = 0, Argument&&... Arg);

		void DestroyEntity(Entity* Entity);

	public:

		Transform* GetTransform(Entity* Entity);
		Camera* GetCamera(Entity* Entity);

	public:

		void DispatchTransformHierarchy();

	public:

		void PrintHierarchy();

	public:

		void Update();

	private:

		U32 mUniqueEntityId = 0;

		std::vector<Entity*> mEntities = {};

		Entity* mRoot = 0;
		Entity* mPlayer = 0;

	private:

		std::map<Entity*, Camera*> mCameras = {};

	private:

		TransformHierarchy mTransformHierarchy = {};
	};
}

namespace engine
{
	template<typename E, typename ... Argument>
	E* Scene::CreateEntity(std::string const& Name, Entity* Parent, Argument&&... Arg)
	{
		if (!Parent)
		{
			Parent = mRoot;
		}

		U32 levelIndex = (Parent) ? (Parent->GetLevelIndex() + 1) : 0;

		mTransformHierarchy.Update(levelIndex);

		U32 transformIndex = mTransformHierarchy.GetNextTransformIndex(levelIndex);

		mTransformHierarchy.IncrementTransformCount(levelIndex);

		Transform* transform = mTransformHierarchy.GetTransform(levelIndex, transformIndex);

		std::memset(transform, 0, sizeof(Transform));

		transform->LocalPosition = R32V3{ 0.0F, 0.0F, 0.0F };
		transform->LocalEulerAngles = R32V3{ 0.0F, 0.0F, 0.0F };
		transform->LocalScale = R32V3{ 1.0F, 1.0F, 1.0F };
		transform->LocalRight = WorldRight;
		transform->LocalUp = WorldUp;
		transform->LocalFront = WorldFront;
		transform->WorldPosition = R32V3{ 0.0F, 0.0F, 0.0F };
		transform->LocalRotation = R32Q{ 0.0F, 0.0F, 0.0F, 1.0F };
		transform->WorldRotation = R32Q{ 0.0F, 0.0F, 0.0F, 1.0F };
		transform->ParentIndex = (Parent) ? Parent->GetTransformIndex() : -1;
		transform->LevelIndex = levelIndex;
		transform->Allocated = 1;
		transform->WorldSpace = 1;

		Entity* entity = mEntities.emplace_back(new E{ Name, Parent, this, transformIndex, mUniqueEntityId, std::forward<Argument>(Arg) ... });

		mUniqueEntityId++;

		return (E*)entity;
	}
}