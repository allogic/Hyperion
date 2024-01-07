#include <Engine/Animation/Animation.h>
#include <Engine/Animation/Bone.h>
#include <Engine/Animation/Skeleton.h>

#include <Engine/Vulkan/Buffer.h>

namespace hyperion
{
	Skeleton::Skeleton()
	{

	}

	Skeleton::~Skeleton()
	{
		
	}

	void Skeleton::ComputeBoneTransformRecursive(Buffer* BoneTransformBuffer, Animation* Animation, R32 Time, R32M4& ParentTransform)
	{
		if (mRootBone)
		{
			mRootBone->ComputeBoneTransformRecursive(BoneTransformBuffer, Animation, Time, ParentTransform);
		}
	}

	void Skeleton::PrintHierarchy(U32 Offset, U32 Indent, U32 Increment)
	{
		if (mRootBone)
		{
			mRootBone->PrintHierarchy(Offset, Indent, Increment);
		}
	}

	void Skeleton::DrawHierarchy()
	{
		if (mRootBone)
		{
			R32M4 transform = R32M4{ 1.0F };

			mRootBone->DrawHierarchy(transform);
		}
	}
}