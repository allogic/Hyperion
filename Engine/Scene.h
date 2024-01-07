#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

#include <Engine/Ecs/Entity.h>
#include <Engine/Ecs/Transform.h>
#include <Engine/Ecs/TransformHierarchy.h>

namespace hyperion
{
	constexpr R32V3 WorldRight = { 1.0F, 0.0F, 0.0F };
	constexpr R32V3 WorldLeft = { -1.0F, 0.0F, 0.0F };
	constexpr R32V3 WorldUp = { 0.0F, 1.0F, 0.0F };
	constexpr R32V3 WorldDown = { 0.0F, -1.0F, 0.0F };
	constexpr R32V3 WorldFront = { 0.0F, 0.0F, 1.0F };
	constexpr R32V3 WorldBack = { 0.0F, 0.0F, -1.0F };

	class Scene
	{
	public:

		inline auto GetRootEntity() const { return mRootEntity; }
		inline auto const& GetEntities() const { return mEntities; }
		inline auto const& GetEntitiesToBeRendered() const { return mEntitiesToBeRendered; }
		inline auto const& GetEntitiesToBeComputed() const { return mEntitiesToBeComputed; }
		inline auto GetPlayer() const { return mPlayerEntity; }

	public:

		Scene();
		virtual ~Scene();

	public:

		bool Load(std::filesystem::path const& File);
		bool Safe(std::filesystem::path const& File);

	public:

		template<typename E, typename ... Argument>
		E* CreateEntity(std::string const& Name, Entity* Parent = 0, Argument&&... Arg);

		Entity* CreateEntityFromModel(Model* Model);

		void DestroyEntity(Entity* Entity);

	public:

		Transform* GetTransform(Entity* Entity);

		CameraComponent* GetCameraComponent(Entity* Entity);

	public:

		void DispatchTransformHierarchy();

	public:

		void PrintHierarchy();

	public:

		void Commit();
		void Update();

	private:

		U32 mUniqueEntityId = 0;

		std::vector<Entity*> mEntities = {};
		std::vector<Entity*> mEntitiesToBeRendered = {};
		std::vector<Entity*> mEntitiesToBeComputed = {};
		std::vector<Entity*> mEntitiesToBeAnimated = {};

		Entity* mRootEntity = 0;
		Entity* mPlayerEntity = 0;

	private:

		std::map<Entity*, CameraComponent*> mCameraComponents = {};

	private:

		TransformHierarchy mTransformHierarchy = {};
	};
}

namespace hyperion
{
	template<typename E, typename ... Argument>
	E* Scene::CreateEntity(std::string const& Name, Entity* Parent, Argument&&... Arg)
	{
		if (!Parent)
		{
			Parent = mRootEntity;
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