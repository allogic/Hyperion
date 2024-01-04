#include <Engine/Scene.h>

#include <Engine/Common/Macros.h>

#include <Engine/Ecs/Transform.h>

#include <Engine/Ecs/Actors/Player.h>

#include <Engine/Ecs/Components/Camera.h>

#include <Engine/Platform/Window.h>

namespace hyperion
{
	Player::Player(std::string const& Name, Entity* Parent, Scene* Scene, U32 TransformIndex, U32 UniqueId) : Entity(Name, Parent, Scene, TransformIndex, UniqueId)
	{
		mCamera = GetScene()->GetCamera(this);
	}

	Player::~Player()
	{

	}

	void Player::Update()
	{
		HandlePosition();
		HandleRotation();

		Entity::Update();
	}

	void Player::HandlePosition()
	{
		Transform* transform = GetScene()->GetTransform(this);

		R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mKeyboardMoveSpeedFast : mKeyboardMoveSpeedNormal;

		if (Window::KeyHeld(KeyCode::KeyD)) mVelocity += transform->LocalRight * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyA)) mVelocity -= transform->LocalRight * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyE)) mVelocity += WorldUp * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyQ)) mVelocity -= WorldUp * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyW)) mVelocity += transform->LocalFront * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyS)) mVelocity -= transform->LocalFront * speed * Window::GetDeltaTime();

		mVelocity += -mVelocity * mMoveDrag * Window::GetDeltaTime();

		transform->LocalPosition += transform->LocalRotation * mVelocity;
	}

	void Player::HandleRotation()
	{
		Transform* transform = GetScene()->GetTransform(this);

		if (Window::RightMouseDown())
		{
			mMousePositionStart = Window::GetMousePosition();
		}

		if (Window::RightMouseHeld() && Window::LeftMouseHeld())
		{
			mMousePositionDelta = mMousePositionStart - Window::GetMousePosition();

			R32V3 positionOffset = {};

			R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mMouseMoveSpeedFast : mMouseMoveSpeedNormal;

			positionOffset += WorldUp * mMousePositionDelta.y * speed * Window::GetDeltaTime();
			positionOffset += transform->LocalRight * mMousePositionDelta.x * speed * Window::GetDeltaTime();

			transform->LocalPosition += transform->LocalRotation * positionOffset;
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