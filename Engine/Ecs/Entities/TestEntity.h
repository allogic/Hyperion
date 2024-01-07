#pragma once

#include <Engine/Ecs/Entity.h>

namespace hyperion
{
	class TestEntity : public Entity
	{
	public:

		TestEntity(std::string const& Name, Entity* Parent, Scene* Scene, U32 TransformIndex, U32 UniqueId);
		virtual ~TestEntity();

	public:

		virtual void Update() override;

	private:

		Transform* mTransform = 0;

	private:

		R32 mAngle = 0.0F;
	};
}