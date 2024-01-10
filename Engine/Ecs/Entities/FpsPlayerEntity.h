#pragma once

#include <Engine/Ecs/Entity.h>

namespace hyperion
{
	class FpsPlayerEntity : public Entity
	{
	public:

		FpsPlayerEntity(EntityArguments const& Arguments);
		virtual ~FpsPlayerEntity();

	public:

		virtual void Update() override;

	private:

		void HandlePosition();
		void HandleRotation();

	private:

		CameraComponent* mCameraComponent = 0;

	private:

		R32V3 mVelocity = {};

		R32V2 mMousePositionStart = {};
		R32V2 mMousePositionDelta = {};

		R32 mKeyboardMoveSpeedFast = 100.0F;
		R32 mKeyboardMoveSpeedNormal = 0.1F;

		R32 mMouseMoveSpeedFast = 5.0F;
		R32 mMouseMoveSpeedNormal = 0.05F;

		R32 mMouseRotationSpeedFast = 5.0F;
		R32 mMouseRotationSpeedNormal = 2.0F;

		R32 mMoveDrag = 5.0F;
		R32 mRotationDrag = 20.0F;
	};
}