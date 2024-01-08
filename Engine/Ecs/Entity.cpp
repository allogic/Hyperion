#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Entity.h>
#include <Engine/Ecs/Transform.h>

namespace hyperion
{
	Entity::Entity(EntityArguments const& Arguments)
	{
		mName = Arguments.Name;
		mParent = Arguments.Parent;
		mScene = Arguments.Scene;
		mAccessor = Arguments.Accessor;
		mUniqueId = Arguments.UniqueId;

		if (mParent)
		{
			mParent->AddChild(this);
		}
	}

	Entity::~Entity()
	{
		for (auto const& [hash, component] : mComponents)
		{
			delete component;
		}

		for (auto const& child : mChildren)
		{
			delete child;
		}
	}

	void Entity::AddChild(Entity* Entity)
	{
		mChildren.emplace_back(Entity);
	}

	void Entity::RemoveChild(Entity* Entity)
	{
		mChildren.erase(std::find(mChildren.begin(), mChildren.end(), Entity));
	}

	void Entity::Update()
	{
		for (auto const& child : mChildren)
		{
			child->Update();
		}
	}

	void Entity::PrintHierarchy(U32 Offset, U32 Indent, U32 Increment)
	{
		if (mParent)
		{
			for (U32 i = 0; i < Offset; ++i)
			{
				LOG(" ");
			}

			for (U32 i = 0; i < Indent; ++i)
			{
				LOG("%c", ((i % Increment == 0) && (i >= Increment)) ? '|' : ' ');
			}
		}

		LOG("%s\n", mName.data());

		for (auto const& child : mChildren)
		{
			child->PrintHierarchy(Offset, Indent + Increment, Increment);
		}

		if (!mParent)
		{
			LOG("\n");
		}
	}
}