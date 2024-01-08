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

#include <Engine/Memory/FixedSizeAccessor.h>

namespace hyperion
{
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

		FixedSizeAccessor* transformAccessor = mTransformHierarchy.AllocateTransform();

		Entity* entity = mEntities.emplace_back(new E{ EntityArguments{ Name, Parent, this, transformAccessor, mUniqueEntityId }, std::forward<Argument>(Arg) ... });

		Transform* transform = entity->GetTransform();

		transform->LocalPosition = R32V3{ 0.0F, 0.0F, 0.0F };
		transform->LocalEulerAngles = R32V3{ 0.0F, 0.0F, 0.0F };
		transform->LocalScale = R32V3{ 1.0F, 1.0F, 1.0F };
		transform->LocalRight = WorldRight;
		transform->LocalUp = WorldUp;
		transform->LocalFront = WorldFront;
		transform->WorldPosition = R32V3{ 0.0F, 0.0F, 0.0F };
		transform->LocalRotation = R32Q{ 0.0F, 0.0F, 0.0F, 1.0F };
		transform->WorldRotation = R32Q{ 0.0F, 0.0F, 0.0F, 1.0F };
		transform->Allocated = 1;
		transform->WorldSpace = 1;
		transform->ParentIndex = (Parent) ? Parent->GetTransformIndex() : -1;

		mUniqueEntityId++;

		return (E*)entity;
	}
}