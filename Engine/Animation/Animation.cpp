#include <Engine/Animation/Animation.h>

#include <Engine/Glm/gtx/quaternion.hpp>

namespace hyperion
{
	Animation::Animation(std::string const& Name, R32 Duration, R32 TicksPerSecond)
	{
		mName = Name;
		mDuration = Duration;
		mTicksPerSecond = TicksPerSecond;
		mLocalTransform = R32M4{ 1.0F };
	}

	Animation::~Animation()
	{

	}

	void Animation::AddPositionKey(Bone* Bone, R32 Time, R32V3 const& Position)
	{
		mPositions[Bone].emplace_back(Time, Position);
	}

	void Animation::AddRotationKey(Bone* Bone, R32 Time, R32Q const& Rotation)
	{
		mRotations[Bone].emplace_back(Time, Rotation);
	}

	void Animation::AddScaleKey(Bone* Bone, R32 Time, R32V3 const& Scale)
	{
		mScales[Bone].emplace_back(Time, Scale);
	}

	R32M4 Animation::ComputeTransform(Bone* Bone, R32 Time)
	{
		R32M4 transform = R32M4{ 1.0F };

		auto positionIt = mPositions.find(Bone);
		auto rotationIt = mRotations.find(Bone);
		auto scaleIt = mScales.find(Bone);

		if ((positionIt != mPositions.end()) && (rotationIt != mRotations.end()) && (scaleIt != mScales.end()))
		{
			R32M4 position = InterpolatePosition(Bone, Time);
			R32M4 rotation = InterpolateRotation(Bone, Time);
			R32M4 scale = InterpolateScale(Bone, Time);

			transform = position * rotation * scale;
		}

		return transform;
	}

	U32 Animation::GetPositionIndex(Bone* Bone, R32 Time)
	{
		for (U32 i = 0; i < ((U32)mPositions[Bone].size() - 1); ++i)
		{
			if (Time < mPositions[Bone][i + 1].Time)
			{
				return i;
			}
		}

		assert(0);

		return 0;
	}

	U32 Animation::GetRotationIndex(Bone* Bone, R32 Time)
	{
		for (U32 i = 0; i < ((U32)mRotations[Bone].size() - 1); ++i)
		{
			if (Time < mRotations[Bone][i + 1].Time)
			{
				return i;
			}
		}

		assert(0);

		return 0;
	}

	U32 Animation::GetScaleIndex(Bone* Bone, R32 Time)
	{
		for (U32 i = 0; i < ((U32)mScales[Bone].size() - 1); ++i)
		{
			if (Time < mScales[Bone][i + 1].Time)
			{
				return i;
			}
		}

		assert(0);

		return 0;
	}

	R32 Animation::GetScaleFactor(R32 LastTime, R32 NextTime, R32 Time)
	{
		R32 midWayLength = Time - LastTime;
		R32 framesDiff = NextTime - LastTime;

		return midWayLength / framesDiff;
	}

	R32M4 Animation::InterpolatePosition(Bone* Bone, R32 Time)
	{
		if (mPositions[Bone].size() == 1)
		{
			return glm::translate(R32M4{ 1.0F }, mPositions[Bone][0].Position);
		}

		U32 p0 = GetPositionIndex(Bone, Time);
		U32 p1 = p0 + 1;

		R32 scaleFactor = GetScaleFactor(mPositions[Bone][p0].Time, mPositions[Bone][p1].Time, Time);

		R32V3 position = glm::mix(mPositions[Bone][p0].Position, mPositions[Bone][p1].Position, scaleFactor);

		return glm::translate(R32M4{ 1.0F }, position);
	}

	R32M4 Animation::InterpolateRotation(Bone* Bone, R32 Time)
	{
		if (mRotations[Bone].size() == 1)
		{
			return glm::toMat4(glm::normalize(mRotations[Bone][0].Rotation));
		}

		U32 p0 = GetRotationIndex(Bone, Time);
		U32 p1 = p0 + 1;

		R32 scaleFactor = GetScaleFactor(mRotations[Bone][p0].Time, mRotations[Bone][p1].Time, Time);

		R32Q rotation = glm::slerp(mRotations[Bone][p0].Rotation, mRotations[Bone][p1].Rotation, scaleFactor);

		return glm::toMat4(glm::normalize(rotation));
	}

	R32M4 Animation::InterpolateScale(Bone* Bone, R32 Time)
	{
		if (mScales[Bone].size() == 1)
		{
			return glm::scale(R32M4{ 1.0F }, mScales[Bone][0].Scale);
		}

		U32 p0 = GetScaleIndex(Bone, Time);
		U32 p1 = p0 + 1;

		R32 scaleFactor = GetScaleFactor(mScales[Bone][p0].Time, mScales[Bone][p1].Time, Time);

		R32V3 scale = glm::mix(mScales[Bone][p0].Scale, mScales[Bone][p1].Scale, scaleFactor);

		return glm::scale(R32M4{ 1.0F }, scale);
	}
}