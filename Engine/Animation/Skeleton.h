#pragma once

#include <string>
#include <map>

#include <Engine/Forward.h>

#include <Engine/Common/Types.h>

namespace hyperion
{
	class Skeleton
	{
	public:

		inline auto GetRootBone() const { return mRootBone; }

	public:

		inline void SetRootBone(Bone* Value) { mRootBone = Value; }

	public:

		Skeleton();
		virtual ~Skeleton();

	public:

		void ComputeBoneTransformRecursive(Buffer* BoneTransformBuffer, Animation* Animation, R32 Time, R32M4& ParentTransform);

	public:

		void PrintHierarchy(U32 Offset = 0, U32 Indent = 0, U32 Increment = 2);
		void DrawHierarchy();

	private:

		Bone* mRootBone = 0;
	};
}