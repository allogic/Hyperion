#pragma once

#include <Engine/Entity.h>

namespace engine
{
	class Test : public Entity
	{
	public:

		Test(std::string const& Name, Entity* Parent, Scene* Scene, U32 TransformIndex, U32 UniqueId);
		virtual ~Test();

	public:

		virtual void Update() override;

	private:

		Camera* mCamera = 0;
		Transform* mTransform = 0;
	};
}