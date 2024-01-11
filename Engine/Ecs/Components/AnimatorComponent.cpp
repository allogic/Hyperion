#include <Engine/Animation/Animation.h>
#include <Engine/Animation/Skeleton.h>

#include <Engine/Common/Random.h>

#include <Engine/Ecs/Components/AnimatorComponent.h>

#include <Engine/Platform/Window.h>

#include <Engine/Renderer/Model.h>

#include <Engine/Vulkan/Buffer.h>

namespace hyperion
{
	AnimatorComponent::AnimatorComponent(ComponentArguments const& Arguments, Model* Model) : Component(Arguments)
	{
		mSharedModel = Model;
		mSharedSkeleton = mSharedModel->GetSkeleton();

		mBoneBuffer = mSharedSkeleton->CopyBoneBuffer();
	}

	AnimatorComponent::~AnimatorComponent()
	{
		delete mBoneBuffer;
	}

	void AnimatorComponent::Play(std::string const& Name)
	{
		auto const& animations = mSharedModel->GetAnimations();

		auto it = animations.find(Name);

		if (it != animations.end())
		{
			Animation* animation = it->second;

			mSharedSkeleton->AddAnimation(this, animation);

			mDuration = animation->GetDuration();
			mTicksPerSecond = animation->GetTicksPerSecond();

			mTime = Random::NextRealBetween(0.0F, mDuration);

			mPlaying = true;
		}
	}

	void AnimatorComponent::Update()
	{
		if (mPlaying)
		{
			mTime += mTicksPerSecond * Window::GetDeltaTime();
			
			if (mTime >= mDuration)
			{
				mTime = 0.0F;
			}
		}
	}
}