#include <Engine/Animation/Bone.h>
#include <Engine/Animation/BoneInfo.h>
#include <Engine/Animation/Skeleton.h>

#include <Engine/Ecs/Components/AnimatorComponent.h>

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
			bone->BoneTransform = R32M4{ 1.0F };
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

	void Skeleton::AddAnimation(AnimatorComponent* AnimatorComponent, Animation* Animation)
	{
		// TODO: Create class for keeping logical indices together

		mAnimationSampler.emplace(AnimatorComponent, Animation);

		U32 descriptorIndex = mBoneHierarchy.AddDescriptorSet();

		mBoneHierarchy.UpdateDescriptorSet(descriptorIndex, AnimatorComponent, Animation);
	}

	Buffer* Skeleton::CopyBoneBuffer()
	{
		return mBoneHierarchy.CopyBoneBuffer();
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
		U32 descriptorIndex = 0;

		for (auto const& [animatorComponent, animation] : mAnimationSampler)
		{
			mBoneHierarchy.Dispatch(descriptorIndex, animatorComponent);

			descriptorIndex++;
		}
	}

	void Skeleton::PrintHierarchy()
	{
		mRootBoneInfo->PrintHierarchy();
	}

	void Skeleton::DrawHierarchy(U32 Color)
	{
		R32M4 worldTransform = R32M4{ 1.0F };

		mRootBoneInfo->DrawHierarchy(worldTransform, Color);
	}
}