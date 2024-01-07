#pragma once

#include <string>
#include <vector>
#include <map>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	struct KeyPosition
	{
		R32 Time;
		R32V3 Position;
	};

	struct KeyRotation
	{
		R32 Time;
		R32Q Rotation;
	};

	struct KeyScale
	{
		R32 Time;
		R32V3 Scale;
	};

	class Animation
	{
	public:

		inline auto GetDuration() const { return mDuration; }
		inline auto GetTicksPerSecond() const { return mTicksPerSecond; }

	public:

		Animation(std::string const& Name, R32 Duration, R32 TicksPerSecond);
		virtual ~Animation();

	public:

		void AddPositionKey(Bone* Bone, R32 Time, R32V3 const& Position);
		void AddRotationKey(Bone* Bone, R32 Time, R32Q const& Rotation);
		void AddScaleKey(Bone* Bone, R32 Time, R32V3 const& Scale);

	public:

		R32M4 ComputeTransform(Bone* Bone, R32 Time);

	public:

		U32 GetPositionIndex(Bone* Bone, R32 Time);
		U32 GetRotationIndex(Bone* Bone, R32 Time);
		U32 GetScaleIndex(Bone* Bone, R32 Time);

	private:

		R32 GetScaleFactor(R32 LastTime, R32 NextTime, R32 Time);

	private:

		R32M4 InterpolatePosition(Bone* Bone, R32 Time);
		R32M4 InterpolateRotation(Bone* Bone, R32 Time);
		R32M4 InterpolateScale(Bone* Bone, R32 Time);

	private:

		std::string mName = "";

		R32 mDuration = 0.0F;

		R32 mTicksPerSecond = 0;

		R32M4 mLocalTransform = {};

	private:

		std::map<Bone*, std::vector<KeyPosition>> mPositions = {};
		std::map<Bone*, std::vector<KeyRotation>> mRotations = {};
		std::map<Bone*, std::vector<KeyScale>> mScales = {};

	private:

		Skeleton* mSkeleton = 0;
	};
}