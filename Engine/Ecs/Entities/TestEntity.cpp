#include <Engine/Ecs/Transform.h>

#include <Engine/Ecs/Entities/TestEntity.h>

#include <Engine/Ecs/Components/CameraComponent.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Renderer.h>

namespace hyperion
{
	TestEntity::TestEntity(EntityArguments const& Arguments) : Entity(Arguments)
	{

	}

	TestEntity::~TestEntity()
	{
		
	}

	void TestEntity::Update()
	{
		Transform* transform = GetTransform();

		mAngle += 6.0F * Window::GetDeltaTime();

		transform->LocalEulerAngles.z = glm::sin(mAngle) * 2.0F;

		Renderer::DrawDebugLine(transform->WorldPosition, transform->WorldPosition + transform->LocalRight, 0xFF0000FF);
		Renderer::DrawDebugLine(transform->WorldPosition, transform->WorldPosition + transform->LocalUp, 0x00FF00FF);
		Renderer::DrawDebugLine(transform->WorldPosition, transform->WorldPosition + transform->LocalFront, 0x0000FFFF);

		Renderer::DrawDebugRectXZ(transform->WorldPosition, R32V3{ 5.0F, 0.0F, 5.0F }, 0xFFFF00FF, transform->WorldRotation);

		Entity::Update();
	}
}