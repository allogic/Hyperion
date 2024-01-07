#include <Engine/Scene.h>

#include <Engine/Ecs/Transform.h>

#include <Engine/Ecs/Entities/TestEntity.h>

#include <Engine/Ecs/Components/CameraComponent.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Renderer.h>

namespace hyperion
{
	TestEntity::TestEntity(std::string const& Name, Entity* Parent, Scene* Scene, U32 TransformIndex, U32 UniqueId) : Entity(Name, Parent, Scene, TransformIndex, UniqueId)
	{
		mTransform = GetScene()->GetTransform(this);
	}

	TestEntity::~TestEntity()
	{
		
	}

	void TestEntity::Update()
	{
		mAngle += 6.0F * Window::GetDeltaTime();

		mTransform->LocalEulerAngles.z = glm::sin(mAngle) * 2.0F;

		Renderer::DrawDebugLine(mTransform->WorldPosition, mTransform->WorldPosition + mTransform->LocalRight, 0xFF0000FF);
		Renderer::DrawDebugLine(mTransform->WorldPosition, mTransform->WorldPosition + mTransform->LocalUp, 0x00FF00FF);
		Renderer::DrawDebugLine(mTransform->WorldPosition, mTransform->WorldPosition + mTransform->LocalFront, 0x0000FFFF);

		Renderer::DrawDebugRectXZ(mTransform->WorldPosition, R32V3{ 5.0F, 0.0F, 5.0F }, 0xFFFF00FF, mTransform->WorldRotation);

		Entity::Update();
	}
}