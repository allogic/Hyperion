#pragma once

#include <string>
#include <vector>
#include <map>
#include <typeinfo>

#include <Common/Types.h>

#include <Engine/Forward.h>

namespace engine
{
	using namespace common;

	class Entity
	{
	public:

		inline auto const& GetName() const { return mName; }
		inline auto GetParent() const { return mParent; }
		inline auto GetScene() const { return mScene; }
		inline auto GetLevelIndex() const { return mLevelIndex; }
		inline auto GetTransformIndex() const { return mTransformIndex; }
		inline auto GetUniqueId() const { return mUniqueId; }
		inline auto const& GetChildren() const { return mChildren; }
		inline auto const& GetComponents() const { return mComponents; }

		template<typename T>
		inline auto GetParent() const { return (T*)mParent; }

	public:

		Entity(std::string const& Name, Entity* Parent, Scene* Scene, U32 TransformIndex, U32 UniqueId);
		virtual ~Entity();

	public:

		void AddChild(Entity* Entity);
		void RemoveChild(Entity* Entity);

	public:

		template<typename C, typename ... Argument>
		C* AttachComponent(Argument&&... Arg);

		template<typename C>
		C* GetComponent();

	public:

		virtual void Update();

	public:

		template<typename Predicate>
		static void TraverseRecursive(Entity* Entity, Predicate&& Pred);

	public:

		void PrintHierarchy(U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);

	private:

		std::string mName = "";

		Entity* mParent = 0;
		Scene* mScene = 0;

		U32 mLevelIndex = 0;
		U32 mTransformIndex = 0;
		U32 mUniqueId = 0;

		std::vector<Entity*> mChildren = {};
		std::map<U64, Component*> mComponents = {};
	};
}

namespace engine
{
	template<typename C, typename ... Argument>
	C* Entity::AttachComponent(Argument&&... Arg)
	{
		mComponents[typeid(C).hash_code()] = new C{ std::forward<Argument>(Arg) ... };

		return (C*)mComponents[typeid(C).hash_code()];
	}

	template<typename C>
	C* Entity::GetComponent()
	{
		return (C*)mComponents[typeid(C).hash_code()];
	}

	template<typename Predicate>
	void Entity::TraverseRecursive(Entity* Entity, Predicate&& Pred)
	{
		Pred(Entity);

		for (auto const& child : mChildren)
		{
			TraverseRecursive(child, Pred);
		}
	}
}