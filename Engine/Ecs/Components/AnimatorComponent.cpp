#include <Engine/Animation/Animation.h>
#include <Engine/Animation/Skeleton.h>

#include <Engine/Ecs/Components/AnimatorComponent.h>

#include <Engine/Platform/Window.h>

#include <Engine/Vulkan/Buffer.h>

namespace hyperion
{
	AnimatorComponent::AnimatorComponent(ComponentArguments const& Arguments) : Component(Arguments)
	{

	}

	AnimatorComponent::~AnimatorComponent()
	{

	}

	void AnimatorComponent::Play(Animation* Animation)
	{
		mSharedAnimation = Animation;
		mSharedSkeleton->SetAnimation(Animation);

		mTime = 0.0F;
		mPlaying = true;
	}

	void AnimatorComponent::Update()
	{
		if (mPlaying)
		{
			Buffer* animationInfoBuffer = mSharedAnimation->GetAnimationInfoBuffer();

			AnimationInfo* animationInfo = animationInfoBuffer->GetMappedData<AnimationInfo>();

			animationInfo->Time += mSharedAnimation->GetTicksPerSecond() * Window::GetDeltaTime();
			
			if (animationInfo->Time >= mSharedAnimation->GetDuration())
			{
				animationInfo->Time = 0.0F;
			}
		}
	}
}