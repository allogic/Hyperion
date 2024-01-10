#include <Engine/Animation/Animation.h>

#include <Engine/Glm/gtx/quaternion.hpp>

#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/BufferVariance.h>

namespace hyperion
{
	Animation::Animation(std::string const& Name, R32 Duration, R32 TicksPerSecond, std::vector<BoneChannelView> const& PositionBoneChannelView, std::vector<BoneChannelView> const& RotationBoneChannelView, std::vector<BoneChannelView> const& ScaleBoneChannelView, std::vector<KeyFramePosition> const& PositionKeyFrames, std::vector<KeyFrameRotation> const& RotationKeyFrames, std::vector<KeyFrameScale> const& ScaleKeyFrames)
	{
		mName = Name;
		mDuration = Duration;
		mTicksPerSecond = TicksPerSecond;

		mAnimationInfoBuffer = BufferVariance::CreateUniformCoherent(sizeof(AnimationInfo));

		mPositionBoneChannelViewBuffer = BufferVariance::CreateStorage((void*)PositionBoneChannelView.data(), sizeof(BoneChannelView) * PositionBoneChannelView.size());
		mRotationBoneChannelViewBuffer = BufferVariance::CreateStorage((void*)RotationBoneChannelView.data(), sizeof(BoneChannelView) * RotationBoneChannelView.size());
		mScaleBoneChannelViewBuffer = BufferVariance::CreateStorage((void*)ScaleBoneChannelView.data(), sizeof(BoneChannelView) * ScaleBoneChannelView.size());

		mPositionKeyFrameBuffer = BufferVariance::CreateStorage((void*)PositionKeyFrames.data(), sizeof(KeyFramePosition) * PositionKeyFrames.size());
		mRotationKeyFrameBuffer = BufferVariance::CreateStorage((void*)RotationKeyFrames.data(), sizeof(KeyFrameRotation) * RotationKeyFrames.size());
		mScaleKeyFrameBuffer = BufferVariance::CreateStorage((void*)ScaleKeyFrames.data(), sizeof(KeyFrameScale) * ScaleKeyFrames.size());
	}

	Animation::~Animation()
	{
		delete mScaleKeyFrameBuffer;
		delete mRotationKeyFrameBuffer;
		delete mPositionKeyFrameBuffer;

		delete mScaleBoneChannelViewBuffer;
		delete mRotationBoneChannelViewBuffer;
		delete mPositionBoneChannelViewBuffer;

		delete mAnimationInfoBuffer;
	}
}