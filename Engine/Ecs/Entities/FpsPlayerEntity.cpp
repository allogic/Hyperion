#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Transform.h>

#include <Engine/Ecs/Entities/FpsPlayerEntity.h>

#include <Engine/Ecs/Components/CameraComponent.h>

#include <Engine/Platform/Window.h>

namespace hyperion
{
	FpsPlayerEntity::FpsPlayerEntity(EntityArguments const& Arguments) : Entity(Arguments)
	{
		mCameraComponent = AttachComponent<CameraComponent>();
	}

	FpsPlayerEntity::~FpsPlayerEntity()
	{

	}

	void FpsPlayerEntity::Update()
	{
		HandlePosition();
		HandleRotation();

		Entity::Update();
	}

	void FpsPlayerEntity::HandlePosition()
	{
		Transform* transform = GetTransform();

		R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mKeyboardMoveSpeedFast : mKeyboardMoveSpeedNormal;

		if (Window::KeyHeld(KeyCode::KeyD)) mVelocity += transform->LocalRight * speed;
		if (Window::KeyHeld(KeyCode::KeyA)) mVelocity -= transform->LocalRight * speed;
		if (Window::KeyHeld(KeyCode::KeyE)) mVelocity += WorldUp * speed;
		if (Window::KeyHeld(KeyCode::KeyQ)) mVelocity -= WorldUp * speed;
		if (Window::KeyHeld(KeyCode::KeyW)) mVelocity += transform->LocalFront * speed;
		if (Window::KeyHeld(KeyCode::KeyS)) mVelocity -= transform->LocalFront * speed;

		mVelocity += -mVelocity * mMoveDrag * Window::GetDeltaTime();

		transform->LocalPosition += transform->LocalRotation * mVelocity * Window::GetDeltaTime();
	}

	void FpsPlayerEntity::HandleRotation()
	{
		Transform* transform = GetTransform();

		if (Window::RightMouseDown())
		{
			mMousePositionStart = Window::GetMousePosition();
		}

		if (Window::RightMouseHeld() && Window::LeftMouseHeld())
		{
			mMousePositionDelta = mMousePositionStart - Window::GetMousePosition();

			R32V3 positionOffset = {};

			R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mMouseMoveSpeedFast : mMouseMoveSpeedNormal;

			positionOffset += WorldUp * mMousePositionDelta.y * speed;
			positionOffset += transform->LocalRight * mMousePositionDelta.x * speed;

			transform->LocalPosition += transform->LocalRotation * positionOffset * Window::GetDeltaTime();
		}
		else if (Window::RightMouseHeld())
		{
			mMousePositionDelta = mMousePositionStart - Window::GetMousePosition();

			R32V3 eulerAngles = transform->LocalEulerAngles;

			R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mMouseRotationSpeedFast : mMouseRotationSpeedNormal;

			eulerAngles.x -= mMousePositionDelta.y * speed * Window::GetDeltaTime();
			eulerAngles.y += mMousePositionDelta.x * speed * Window::GetDeltaTime();

			if (eulerAngles.x < -89.9F) eulerAngles.x = -89.9F;
			if (eulerAngles.x > 89.9F) eulerAngles.x = 89.9F;

			transform->LocalEulerAngles = eulerAngles;
		}

		mMousePositionStart -= mMousePositionDelta * mRotationDrag * Window::GetDeltaTime();
	}
}