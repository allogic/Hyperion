#pragma once

#include <string>
#include <vector>
#include <map>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	struct AnimationInfo
	{
		R32 Time;
	};

	struct BoneChannelView
	{
		U32 Offset;
		U32 Count;
	};

	struct KeyFramePosition
	{
		R32V3 Position;
		R32 Time;
	};

	struct KeyFrameRotation
	{
		R32Q Rotation;
		R32 Time;
		U32 Reserved0;
		U32 Reserved1;
		U32 Reserved2;
	};

	struct KeyFrameScale
	{
		R32V3 Scale;
		R32 Time;
	};

	class Animation
	{
	public:

		inline auto GetDuration() const { return mDuration; }
		inline auto GetTicksPerSecond() const { return mTicksPerSecond; }
		inline auto GetAnimationInfoBuffer() const { return mAnimationInfoBuffer; }
		inline auto GetPositionBoneChannelViewBuffer() const { return mPositionBoneChannelViewBuffer; }
		inline auto GetRotationBoneChannelViewBuffer() const { return mRotationBoneChannelViewBuffer; }
		inline auto GetScaleBoneChannelViewBuffer() const { return mScaleBoneChannelViewBuffer; }
		inline auto GetPositionKeyFrameBuffer() const { return mPositionKeyFrameBuffer; }
		inline auto GetRotationKeyFrameBuffer() const { return mRotationKeyFrameBuffer; }
		inline auto GetScaleKeyFrameBuffer() const { return mScaleKeyFrameBuffer; }

	public:

		Animation(std::string const& Name, R32 Duration, R32 TicksPerSecond, std::vector<BoneChannelView> const& PositionBoneChannelView, std::vector<BoneChannelView> const& RotationBoneChannelView, std::vector<BoneChannelView> const& ScaleBoneChannelView, std::vector<KeyFramePosition> const& PositionKeyFrames, std::vector<KeyFrameRotation> const& RotationKeyFrames, std::vector<KeyFrameScale> const& ScaleKeyFrames);
		virtual ~Animation();

	private:

		std::string mName = "";

		R32 mDuration = 0.0F;
		R32 mTicksPerSecond = 0.0F;

	private:

		Buffer* mAnimationInfoBuffer = 0;

		Buffer* mPositionBoneChannelViewBuffer = 0;
		Buffer* mRotationBoneChannelViewBuffer = 0;
		Buffer* mScaleBoneChannelViewBuffer = 0;

		Buffer* mPositionKeyFrameBuffer = 0;
		Buffer* mRotationKeyFrameBuffer = 0;
		Buffer* mScaleKeyFrameBuffer = 0;
	};
}