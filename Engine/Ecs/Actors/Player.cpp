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
		mTransform = GetScene()->GetTransform(this);

		mTransform->LocalPosition = R32V3{ 0.0F, 0.0F, 0.0F };
		mTransform->LocalEulerAngles = R32V3{ 0.0F, 0.0F, 0.0F };
		mTransform->LocalScale = R32V3{ 0.0F, 0.0F, 0.0F };
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
		static R32V3 velocity = {};

		R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mKeyboardMoveSpeedFast : mKeyboardMoveSpeedNormal;

		if (Window::KeyHeld(KeyCode::KeyD)) velocity += mTransform->LocalRight * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyA)) velocity -= mTransform->LocalRight * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyE)) velocity += WorldUp * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyQ)) velocity -= WorldUp * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyW)) velocity += mTransform->LocalFront * speed * Window::GetDeltaTime();
		if (Window::KeyHeld(KeyCode::KeyS)) velocity -= mTransform->LocalFront * speed * Window::GetDeltaTime();

		velocity += -velocity * mMoveDrag * Window::GetDeltaTime();

		mTransform->LocalPosition += (mTransform->LocalRotation * velocity);
	}

	void Player::HandleRotation()
	{
		static R32V2 mousePositionStart = {};
		static R32V2 mousePositionDelta = {};

		if (Window::RightMouseDown())
		{
			mousePositionStart = Window::GetMousePosition();
		}

		if (Window::RightMouseHeld() && Window::LeftMouseHeld())
		{
			mousePositionDelta = mousePositionStart - Window::GetMousePosition();

			R32V3 positionOffset = {};

			R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mMouseMoveSpeedFast : mMouseMoveSpeedNormal;

			positionOffset += WorldRight * mousePositionDelta.x * speed * Window::GetDeltaTime();
			positionOffset += WorldUp * mousePositionDelta.y * speed * Window::GetDeltaTime();

			mTransform->LocalPosition += positionOffset;
		}
		else if (Window::RightMouseHeld())
		{
			mousePositionDelta = mousePositionStart - Window::GetMousePosition();

			R32V3 eulerAngles = mTransform->LocalEulerAngles;

			R32 speed = Window::KeyHeld(KeyCode::LeftShift) ? mMouseRotationSpeedFast : mMouseRotationSpeedNormal;

			eulerAngles.x -= mousePositionDelta.y * speed * Window::GetDeltaTime();
			eulerAngles.y += mousePositionDelta.x * speed * Window::GetDeltaTime();

			if (eulerAngles.x < -89.9F) eulerAngles.x = -89.9F;
			if (eulerAngles.x > 89.9F) eulerAngles.x = 89.9F;

			mTransform->LocalEulerAngles = eulerAngles;
		}

		mousePositionStart -= mousePositionDelta * mRotationDrag * Window::GetDeltaTime();
	}
}