#include <Engine/Scene.h>

#include <Engine/Actors/Test.h>

#include <Engine/Components/Camera.h>

#include <Vulkan/Window.h>
#include <Vulkan/Renderer.h>

namespace engine
{
	Test::Test(std::string const& Name, Entity* Parent, Scene* Scene, U32 TransformIndex, U32 UniqueId) : Entity(Name, Parent, Scene, TransformIndex, UniqueId)
	{
		mCamera = GetScene()->GetCamera(this);
		mTransform = GetScene()->GetTransform(this);
	}

	Test::~Test()
	{
		
	}

	void Test::Update()
	{
		static R32 angle = 0.0F;

		angle += 0.0001F;

		mTransform->LocalEulerAngles.z = glm::sin(angle) * 2.0F;

		Renderer::DrawDebugLine(mTransform->WorldPosition, mTransform->WorldPosition + mTransform->LocalRight, 0xFF0000FF);
		Renderer::DrawDebugLine(mTransform->WorldPosition, mTransform->WorldPosition + mTransform->LocalUp, 0x00FF00FF);
		Renderer::DrawDebugLine(mTransform->WorldPosition, mTransform->WorldPosition + mTransform->LocalFront, 0x0000FFFF);

		Renderer::DrawDebugRectXZ(mTransform->WorldPosition, R32V3{ 5.0F, 0.0F, 5.0F }, 0xFFFF00FF, mTransform->WorldRotation);

		Entity::Update();
	}
}