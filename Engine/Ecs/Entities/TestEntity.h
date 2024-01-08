#pragma once

#include <Engine/Ecs/Entity.h>

namespace hyperion
{
	class TestEntity : public Entity
	{
	public:

		TestEntity(EntityArguments const& Arguments);
		virtual ~TestEntity();

	public:

		virtual void Update() override;

	private:

		R32 mAngle = 0.0F;
	};
}