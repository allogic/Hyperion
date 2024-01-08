#pragma once

#include <string>
#include <vector>
#include <map>
#include <typeinfo>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

#include <Engine/Ecs/Component.h>

#include <Engine/Memory/FixedSizeAccessor.h>

namespace hyperion
{
	struct EntityArguments
	{
		std::string Name;
		Entity* Parent;
		Scene* Scene;
		FixedSizeAccessor* Accessor;
		U32 UniqueId;
	};

	class Entity
	{
	public:

		inline auto const& GetName() const { return mName; }
		inline auto GetParent() const { return mParent; }
		inline auto GetScene() const { return mScene; }
		inline auto GetTransform() const { return mAccessor->GetData<Transform>(); }
		inline auto GetTransformIndex() const { return mAccessor->GetIndex(); }
		inline auto GetUniqueId() const { return mUniqueId; }
		inline auto const& GetChildren() const { return mChildren; }
		inline auto const& GetComponents() const { return mComponents; }

		template<typename T>
		inline auto GetParent() const { return (T*)mParent; }

	public:

		Entity(EntityArguments const& Arguments);
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

		template<typename Function>
		static void TraverseRecursive(Entity* Entity, Function const& Func);

	public:

		void PrintHierarchy(U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);

	private:

		std::string mName = "";

		Entity* mParent = 0;
		Scene* mScene = 0;
		FixedSizeAccessor* mAccessor = 0;
		U32 mUniqueId = 0;

		std::vector<Entity*> mChildren = {};
		std::map<U64, Component*> mComponents = {};
	};
}

namespace hyperion
{
	template<typename C, typename ... Argument>
	C* Entity::AttachComponent(Argument&&... Arg)
	{
		mComponents[typeid(C).hash_code()] = new C{ ComponentArguments{ 0 }, std::forward<Argument>(Arg) ... };

		return (C*)mComponents[typeid(C).hash_code()];
	}

	template<typename C>
	C* Entity::GetComponent()
	{
		return (C*)mComponents[typeid(C).hash_code()];
	}

	template<typename Function>
	void Entity::TraverseRecursive(Entity* Entity, Function const& Func)
	{
		Func(Entity);

		for (auto const& child : mChildren)
		{
			TraverseRecursive(child, Func);
		}
	}
}