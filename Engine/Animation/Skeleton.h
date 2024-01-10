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

		inline auto GetBuffer() const { return mBoneHierarchy.GetBuffer(); }

	public:

		Skeleton();
		virtual ~Skeleton();

	public:

		BoneInfo* CreateBone(std::string const& Name, std::string const& ParentName, R32M4 const& Transform, R32M4 const& Offset);
		BoneInfo* FindBone(std::string const& Name);

	public:

		void SetAnimation(Animation* Animation);

	public:

		void BuildBoneHierarchy();
		void DispatchBoneHierarchy();

	public:

		void PrintHierarchy();

	private:

		std::map<std::string, BoneInfo*> mBoneInfos = {};

		BoneInfo* mRootBoneInfo = 0;

	private:

		BoneHierarchy mBoneHierarchy = {};
	};
}