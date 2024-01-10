#include <Engine/Animation/Bone.h>
#include <Engine/Animation/BoneInfo.h>

#include <Engine/Common/Macros.h>

namespace hyperion
{
	BoneInfo::BoneInfo(BoneInfoArguments const& Arguments)
	{
		mName = Arguments.Name;
		mParentName = Arguments.ParentName;
		mAccessor = Arguments.Accessor;
	}

	BoneInfo::~BoneInfo()
	{
		for (auto const& child : mChildren)
		{
			delete child;
		}
	}

	void BoneInfo::AddChild(BoneInfo* BoneInfo)
	{
		mChildren.emplace_back(BoneInfo);
	}

	void BoneInfo::UpdateParentIndices()
	{
		if (mParent)
		{
			Bone* bone = mAccessor->GetData<Bone>();

			bone->ParentIndex = mParent->GetBoneIndex();
		}

		for (const auto& child : mChildren)
		{
			child->UpdateParentIndices();
		}
	}

	void BoneInfo::PrintHierarchy(U32 Offset, U32 Indent, U32 Increment)
	{
		if (mParent)
		{
			for (U32 i = 0; i < Offset; ++i)
			{
				LOG(" ");
			}

			for (U32 i = 0; i < Indent; ++i)
			{
				LOG("%c", ((i % Increment == 0) && (i >= Increment)) ? '|' : ' ');
			}
		}

		LOG("%s\n", mName.data());

		for (auto const& child : mChildren)
		{
			child->PrintHierarchy(Offset, Indent + Increment, Increment);
		}

		if (!mParent)
		{
			LOG("\n");
		}
	}
}