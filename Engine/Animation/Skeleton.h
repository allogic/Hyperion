#pragma once

#include <string>
#include <map>

#include <Engine/Animation/BoneHierarchy.h>

#include <Engine/Common/Types.h>

#include <Engine/Memory/FixedSizeAccessor.h>

namespace hyperion
{
	class Skeleton
	{
	public:

		inline auto GetBoneBuffer() const { return mBoneHierarchy.GetBoneBuffer(); }

	public:

		Skeleton();
		virtual ~Skeleton();

	public:

		BoneInfo* CreateBone(std::string const& Name, std::string const& ParentName, R32M4 const& Transform, R32M4 const& Offset);
		BoneInfo* FindBone(std::string const& Name);

	public:

		void AddAnimation(AnimatorComponent* AnimatorComponent, Animation* Animation);

	public:

		Buffer* CopyBoneBuffer();

	public:

		void BuildBoneHierarchy();
		void DispatchBoneHierarchy();

	public:

		void PrintHierarchy();
		void DrawHierarchy(U32 Color);

	private:

		std::map<std::string, BoneInfo*> mBoneInfos = {};
		std::map<AnimatorComponent*, Animation*> mAnimationSampler = {};

		BoneInfo* mRootBoneInfo = 0;

	private:

		BoneHierarchy mBoneHierarchy = {};
	};
}