#include <Engine/Animation/Bone.h>
#include <Engine/Animation/BoneInfo.h>
#include <Engine/Animation/Skeleton.h>

namespace hyperion
{
	Skeleton::Skeleton()
	{

	}

	Skeleton::~Skeleton()
	{
		delete mRootBoneInfo;
	}

	BoneInfo* Skeleton::CreateBone(std::string const& Name, std::string const& ParentName, R32M4 const& Transform, R32M4 const& Offset)
	{
		auto it = mBoneInfos.find(Name);

		if (it == mBoneInfos.end())
		{
			FixedSizeAccessor* boneAccessor = mBoneHierarchy.AllocateBone();

			BoneInfo* boneInfo = mBoneInfos[Name] = new BoneInfo{ BoneInfoArguments{ Name, ParentName, boneAccessor } };

			Bone* bone = boneInfo->GetBone();

			bone->ChannelViewIndex = 0;
			bone->Allocated = 1;
			bone->ParentIndex = -1;
			bone->ParentTransform = R32M4{ 1.0F };
			bone->OffsetTransform = Offset;
			bone->LocalTransform = Transform;
			bone->WorldTransform = R32M4{ 1.0F };

			return boneInfo;
		}
		else
		{
			return it->second;
		}
	}

	BoneInfo* Skeleton::FindBone(std::string const& Name)
	{
		auto it = mBoneInfos.find(Name);

		if (it == mBoneInfos.end())
		{
			return 0;
		}
		else
		{
			return it->second;
		}
	}

	void Skeleton::SetAnimation(Animation* Animation)
	{
		mBoneHierarchy.UpdateDescriptorSet(Animation);
	}

	void Skeleton::BuildBoneHierarchy()
	{
		auto boneInfosCopy = mBoneInfos;
		
		mRootBoneInfo = CreateBone("Root", "", R32M4{ 1.0F }, R32M4{ 1.0F });
		
		for (auto it = boneInfosCopy.begin(); it != boneInfosCopy.end();)
		{
			BoneInfo* boneInfo = it->second;
			BoneInfo* parentBoneInfo = FindBone(boneInfo->GetParentName());
		
			if (parentBoneInfo)
			{
				parentBoneInfo->AddChild(boneInfo);
		
				boneInfo->SetParent(parentBoneInfo);
			}
			else
			{
				mRootBoneInfo->AddChild(boneInfo);
		
				boneInfo->SetParent(mRootBoneInfo);
			}
		
			it = boneInfosCopy.erase(it);
		}

		mRootBoneInfo->UpdateParentIndices();
	}

	void Skeleton::DispatchBoneHierarchy()
	{
		mBoneHierarchy.Dispatch();
	}

	void Skeleton::PrintHierarchy()
	{
		mRootBoneInfo->PrintHierarchy();
	}
}