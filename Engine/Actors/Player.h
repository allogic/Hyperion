#pragma once

#include <Engine/Entity.h>

namespace engine
{
	class Player : public Entity
	{
	public:

		Player(std::string const& Name, Entity* Parent, Scene* Scene, U32 TransformIndex, U32 UniqueId);
		virtual ~Player();

	public:

		virtual void Update() override;

	private:

		void HandlePosition();
		void HandleRotation();

	private:

		Camera* mCamera = 0;
		Transform* mTransform = 0;

	private:

		R32 mKeyboardMoveSpeedFast = 100.0F;
		R32 mKeyboardMoveSpeedNormal = 5.0F;

		R32 mMouseMoveSpeedFast = 5.0F;
		R32 mMouseMoveSpeedNormal = 0.1F;

		R32 mMouseRotationSpeedFast = 5.0F;
		R32 mMouseRotationSpeedNormal = 2.0F;

		R32 mMoveDrag = 5.0F;
		R32 mRotationDrag = 20.0F;
	};
}