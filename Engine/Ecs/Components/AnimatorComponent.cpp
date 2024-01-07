#include <Engine/Animation/Animation.h>
#include <Engine/Animation/Skeleton.h>

#include <Engine/Ecs/Components/AnimatorComponent.h>

#include <Engine/Platform/Window.h>

#include <Engine/Vulkan/Buffer.h>

namespace hyperion
{
	AnimatorComponent::AnimatorComponent()
	{

	}

	AnimatorComponent::~AnimatorComponent()
	{
		delete mBoneTransformBuffer;
	}

	void AnimatorComponent::Play(Animation* Animation)
	{
		mAnimation = Animation;
		mTime = 0.0F;
	}

	void AnimatorComponent::Update()
	{
		if (mAnimation)
		{
			mTime += mAnimation->GetTicksPerSecond() * Window::GetDeltaTime();

			mTime = fmod(mTime, mAnimation->GetDuration()); // TODO: Remove fmod

			R32M4 transform = R32M4{ 1.0F };

			mSharedSkeleton->ComputeBoneTransformRecursive(mBoneTransformBuffer, mAnimation, mTime, transform);
		}
	}
}